# srcs/python/api.py
import os
import time
import json
import requests
from requests.exceptions import ConnectionError, Timeout, RequestException
from typing import Generator, Dict, List, Optional

class OllamaAPI:
    def __init__(self, model="codellama"):
        """
        Initialize the OllamaAPI class.
        Args:
            model (str): The model to use for the API.
        """
        self.base_url = os.environ.get("OLLAMA_API_BASE", "http://localhost:11434")
        self.model = model

    def generate(self, prompt: str, max_retries: int = 3, delay: int = 1, 
                timeout: int = 30, stream: bool = False) -> str | Generator[str, None, None]:
        """
        Generate a response from the Ollama API.
        """
        for attempt in range(max_retries):
            try:
                if stream:
                    return self._generate_stream(prompt, timeout)
                else:
                    return self._generate_sync(prompt, timeout)
            except (ConnectionError, Timeout) as e:
                if attempt == max_retries - 1:
                    raise ConnectionError(f"Failed to connect to Ollama API after {max_retries} attempts.")
                print(f"Connection error: {e}. Retrying in {delay} seconds...")
                time.sleep(delay)
                delay *= 2

    def _generate_sync(self, prompt: str, timeout: int) -> str:
        """동기 방식으로 응답 생성"""
        response = requests.post(
            f"{self.base_url}/api/generate",
            json={"model": self.model, "prompt": prompt, "stream": False},
            timeout=timeout
        )

        if response.status_code != 200:
            raise ValueError(f"Error: {response.status_code} - {response.text}")

        try:
            return response.json()['response']
        except (KeyError, ValueError) as e:
            raise ValueError(f"Invalid response format: {response.text}")

    def _generate_stream(self, prompt: str, timeout: int) -> Generator[str, None, None]:
        """스트리밍 방식으로 응답 생성"""
        response = requests.post(
            f"{self.base_url}/api/generate",
            json={"model": self.model, "prompt": prompt, "stream": True},
            timeout=timeout,
            stream=True
        )

        if response.status_code != 200:
            raise ValueError(f"Error: {response.status_code} - {response.text}")

        for line in response.iter_lines():
            if line:
                chunk = line.decode('utf-8')
                try:
                    json_data = json.loads(chunk)
                    if 'response' in json_data and json_data['response']:
                        yield json_data['response']
                    if json_data.get('done', False):
                        break
                except json.JSONDecodeError:
                    continue
                except Exception:
                    continue

    def generate_async(self, prompt: str, timeout: int = 30) -> str:
        """웹 API용 비동기 안전한 응답 생성 (실제로는 동기이지만 웹에서 사용하기 편함)"""
        return self._generate_sync(prompt, timeout)

    def list_models(self) -> List[Dict]:
        """
        List available models from the Ollama API.
        """
        try:
            response = requests.get(f"{self.base_url}/api/tags", timeout=10)
            if response.status_code == 200:
                return response.json().get("models", [])
            else:
                raise ValueError(f"Error: {response.status_code}")
        except RequestException as e:
            raise ConnectionError(f"Failed to connect to Ollama API: {e}")

    def check_connection(self) -> bool:
        """
        Check if the Ollama API is reachable.
        """
        try:
            response = requests.get(f"{self.base_url}/api/tags", timeout=5)
            return response.status_code == 200
        except RequestException:
            return False

    def get_model_info(self, model_name: Optional[str] = None) -> Dict:
        """모델 정보 조회"""
        model = model_name or self.model
        try:
            response = requests.post(f"{self.base_url}/api/show", 
                                   json={"name": model}, timeout=10)
            if response.status_code == 200:
                return response.json()
            return {}
        except RequestException:
            return {}