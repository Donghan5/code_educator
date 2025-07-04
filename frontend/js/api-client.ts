const API_BASE_URL = window.location.hostname === 'localhost' 
  ? 'http://localhost:8000' 
  : '/api';

export interface QuestionRequest {
  question: string;
  model?: string;
  language?: string;
  context?: string;
}

export interface QuestionResponse {
  response: string;
  model_used: string;
}

export interface CodeExplainRequest {
  code: string;
  language?: string;
  model?: string;
}

export interface CodeGenerateRequest {
  description: string;
  language: string;
  model?: string;
}

export interface DebugRequest {
  code: string;
  error_message?: string;
  language?: string;
  model?: string;
}

export interface AnalyzeRequest {
  code: string;
  language?: string;
  ai_analysis?: boolean;
  model?: string;
}

export interface AnalysisResult {
  language: string;
  complexity: number;
  imports: string[];
  functions: string[];
  classes: string[];
  line_count: number;
  comment_count: number;
  comment_ratio: number;
  nesting_depth: number;
  cyclomatic_complexity: number;
  potential_issues: string[];
  suggestions: string[];
  quality_score: number;
  metadata: Record<string, any>;
  ai_analysis?: string;
  file_path?: string;
  file_name?: string;
}

export interface ModelInfo {
  name: string;
  size?: string;
  modified_at?: string;
}

export interface HealthStatus {
  status: string;
  ollama_connected: boolean;
  core_module: boolean;
  supported_languages: string[];
  features: Record<string, boolean>;
}

export interface ErrorResponse {
  error: string;
  detail?: string;
  code?: string;
}

// API client class
export class CodeEducatorAPI {
  private baseUrl: string;
  private defaultHeaders: HeadersInit;

  constructor(baseUrl: string = API_BASE_URL) {
    this.baseUrl = baseUrl;
    this.defaultHeaders = {
      'Content-Type': 'application/json',
    };
  }

  // error handler
  private async handleResponse<T>(response: Response): Promise<T> {
    if (!response.ok) {
      const error: ErrorResponse = await response.json().catch(() => ({
        error: 'Unknown error',
        detail: response.statusText,
        code: response.status.toString()
      }));
      throw new Error(error.detail || error.error);
    }
    return response.json();
  }

  async checkHealth(): Promise<HealthStatus> {
    const response = await fetch(`${this.baseUrl}/health`);
    return this.handleResponse<HealthStatus>(response);
  }

  async getModels(): Promise<ModelInfo[]> {
    const response = await fetch(`${this.baseUrl}/models`);
    const data = await this.handleResponse<{ models: ModelInfo[] }>(response);
    return data.models;
  }

  async askQuestion(request: QuestionRequest): Promise<QuestionResponse> {
    const response = await fetch(`${this.baseUrl}/ask`, {
      method: 'POST',
      headers: this.defaultHeaders,
      body: JSON.stringify(request),
    });
    return this.handleResponse<QuestionResponse>(response);
  }

  async explainCode(request: CodeExplainRequest): Promise<QuestionResponse> {
    const response = await fetch(`${this.baseUrl}/explain`, {
      method: 'POST',
      headers: this.defaultHeaders,
      body: JSON.stringify(request),
    });
    return this.handleResponse<QuestionResponse>(response);
  }

  async generateCode(request: CodeGenerateRequest): Promise<QuestionResponse> {
    const response = await fetch(`${this.baseUrl}/generate`, {
      method: 'POST',
      headers: this.defaultHeaders,
      body: JSON.stringify(request),
    });
    return this.handleResponse<QuestionResponse>(response);
  }

  async debugCode(request: DebugRequest): Promise<QuestionResponse> {
    const response = await fetch(`${this.baseUrl}/debug`, {
      method: 'POST',
      headers: this.defaultHeaders,
      body: JSON.stringify(request),
    });
    return this.handleResponse<QuestionResponse>(response);
  }

  async analyzeCode(request: AnalyzeRequest): Promise<AnalysisResult> {
    const response = await fetch(`${this.baseUrl}/analyze`, {
      method: 'POST',
      headers: this.defaultHeaders,
      body: JSON.stringify(request),
    });
    return this.handleResponse<AnalysisResult>(response);
  }

  async analyzeFile(file: File, aiAnalysis: boolean = false, model: string = 'codellama'): Promise<AnalysisResult> {
    const formData = new FormData();
    formData.append('file', file);
    formData.append('ai_analysis', aiAnalysis.toString());
    formData.append('model', model);

    const response = await fetch(`${this.baseUrl}/analyze/file`, {
      method: 'POST',
      body: formData,
    });
    return this.handleResponse<AnalysisResult>(response);
  }

  async checkQuality(code: string, threshold: number = 70): Promise<{
    score: number;
    threshold: number;
    passed: boolean;
    issues: string[];
    suggestions: string[];
  }> {
    const response = await fetch(`${this.baseUrl}/analyze/quality/${threshold}?code=${encodeURIComponent(code)}`);
    return this.handleResponse(response);
  }

  async getSupportedLanguages(): Promise<{
    languages: string[];
    core_module_available: boolean;
  }> {
    const response = await fetch(`${this.baseUrl}/languages`);
    return this.handleResponse(response);
  }

  async getSystemStats(): Promise<any> {
    const response = await fetch(`${this.baseUrl}/stats`);
    return this.handleResponse(response);
  }
}

export const api = new CodeEducatorAPI();

export const utils = {
  getLanguageFromExtension(filename: string): string {
    const ext = filename.split('.').pop()?.toLowerCase();
    const mapping: Record<string, string> = {
      'py': 'python',
      'js': 'javascript',
      'ts': 'javascript',
      'cpp': 'cpp',
      'cc': 'cpp',
      'c': 'c',
      'h': 'c',
      'hpp': 'cpp',
    };
    return mapping[ext || ''] || 'unknown';
  },

  getQualityColor(score: number): string {
    if (score >= 80) return '#4CAF50';
    if (score >= 60) return '#FFC107';
    if (score >= 40) return '#FF9800';
    return '#F44336';
  },

  getQualityGrade(score: number): string {
    if (score >= 90) return 'A+';
    if (score >= 80) return 'A';
    if (score >= 70) return 'B';
    if (score >= 60) return 'C';
    if (score >= 50) return 'D';
    return 'F';
  },

  getComplexityLevel(complexity: number): string {
    if (complexity <= 5) return '매우 간단';
    if (complexity <= 10) return '간단';
    if (complexity <= 20) return '보통';
    if (complexity <= 40) return '복잡';
    return '매우 복잡';
  },

  formatFileSize(bytes: number): string {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
  },

  getHighlightClass(language: string): string {
    const mapping: Record<string, string> = {
      'python': 'language-python',
      'javascript': 'language-javascript',
      'cpp': 'language-cpp',
      'c': 'language-c',
      'unknown': 'language-plaintext'
    };
    return mapping[language] || 'language-plaintext';
  }
};

export const examples = {
  async askSimpleQuestion() {
    try {
      const response = await api.askQuestion({
        question: "파이썬에서 리스트와 튜플의 차이점은?",
        language: "python"
      });
      console.log(response.response);
    } catch (error) {
      console.error('Error:', error);
    }
  },

  async analyzeCodeExample() {
    const code = `
def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)
`;
    try {
      const result = await api.analyzeCode({
        code,
        ai_analysis: true
      });
      console.log('분석 결과:', result);
      console.log('품질 점수:', result.quality_score);
      console.log('잠재적 문제:', result.potential_issues);
    } catch (error) {
      console.error('Error:', error);
    }
  },

  async analyzeFileExample(file: File) {
    try {
      const result = await api.analyzeFile(file, true);
      console.log(`${file.name} 분석 완료`);
      console.log('언어:', result.language);
      console.log('품질:', utils.getQualityGrade(result.quality_score));
    } catch (error) {
      console.error('Error:', error);
    }
  }
};

// 기본 export
export default CodeEducatorAPI;