"""
	Ollama APT transport module for Python-APT
"""
import os
import time
import requests
from requests.exceptions import ConnectionError, Timeout, RequestException

class OllamaAPI:
    def __init__(self, model="codellama"):
        """
        Initialize the OllamaAPI class.
		Args:
			model (str): The model to use for the API.
		"""
        self.base_url = os.environ.get("OLLAMA_API_URL", "http://localhost:11434")
        self.model = model

    def generate(self, prompt, max_retries=3, delay=1, timeout=30, stream=False):
        """
          Generate a response from the Ollama API.
        """
        for attempt in range(max_retries):
            try:
                if stream:
                    return self._generate_stream(prompt, timeout)
                else:
                    return self._generate(prompt, timeout)
            except (ConnectionError, Timeout) as e:
                if attempt == max_retries - 1:
                    raise ConnectionError(f"Failed to connect to Ollama API after {max_retries} attempts.")
                print(f"Connection error: {e}. Retrying in {delay} seconds...")
                time.sleep(delay)
                delay *= 2

    def _generate_sync(self, prompt, timeout):
        response = requests.post(
            f"{self.base_url}/generate",
            json={"model": self.model, "prompt": prompt, "stream": False},
            timeout=timeout
        )

        if response.status_code != 200:
            raise ValueError(f"Error: {response.status_code} - {response.text}")

        try:
            return response.json()['response']
        except (KeyError, ValueError) as e:
            raise ValueError(f"Invalid response format: {response.text}")

    def _generate_stream(self, prompt, timeout):
        response = requests.post(
            f"{self.base_url}/generate",
            json={"model": self.model, "prompt": prompt, "stream": True},
            timeout=timeout,
            stream=True
        )

        if response.status_code != 200:
            raise ValueError(f"Error: {response.status_code} - {response.text}")

        for line in response.iter_lines():
            if line:
                try:
                    chunk = line.decode('utf-8')
                    if chunk.startswith("data: "):
                        data = chunk[6:]
                    if data == "[DONE]":
                        break
                    yield data
                except Exception as e:
                    raise ValueError(f"Error decoding response: {e}")

    def list_models(self):
        """
        List available models from the Ollama API.
        """
        response = requests.get(f"{self.base_url}/api/tags")
        try:
            if response.status_code == 200:
                return response.json().get("models", [])
            else:
                raise ValueError(f"Error: {response.status_code}")
        except RequestException as e:
            raise ConnectionError(f"Failed to connect to Ollama API: {e}")

    def check_connection(self):
        """
        Check if the Ollama API is reachable.
        """
        try:
            response = requests.get(f"{self.base_url}/api/tags", timeout=5)
            return response.status_code == 200
        except RequestException:
           return False
