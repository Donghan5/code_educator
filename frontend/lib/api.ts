import axios from 'axios';

const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8000';

const api = axios.create({
  baseURL: API_BASE_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

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
  ai_analysis?: string;
}

export interface Model {
  name: string;
  size: string;
}

export class CodeEducatorAPI {
  static async healthCheck() {
    const response = await api.get('/health');
    return response.data;
  }

  static async getModels(): Promise<Model[]> {
    const response = await api.get('/models');
    return response.data.models;
  }

  static async askQuestion(question: string, model: string = 'codellama') {
    const response = await api.post('/ask', {
      question,
      model,
      stream: false,
    });
    return response.data.response;
  }

  static async analyzeCode(
    code: string,
    aiAnalysis: boolean = false,
    model: string = 'codellama'
  ): Promise<AnalysisResult> {
    const response = await api.post('/analyze', {
      code,
      ai_analysis: aiAnalysis,
      model,
    });
    return response.data;
  }

  static async analyzeFile(file: File): Promise<AnalysisResult> {
    const formData = new FormData();
    formData.append('file', file);
    
    const response = await api.post('/analyze/file', formData, {
      headers: {
        'Content-Type': 'multipart/form-data',
      },
    });
    return response.data;
  }
}

export default CodeEducatorAPI;