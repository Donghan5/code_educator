var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
const API_BASE_URL = window.location.hostname === 'localhost'
    ? 'http://localhost:8000'
    : '/api';
// API client class
export class CodeEducatorAPI {
    constructor(baseUrl = API_BASE_URL) {
        this.baseUrl = baseUrl;
        this.defaultHeaders = {
            'Content-Type': 'application/json',
        };
    }
    // error handler
    handleResponse(response) {
        return __awaiter(this, void 0, void 0, function* () {
            if (!response.ok) {
                const error = yield response.json().catch(() => ({
                    error: 'Unknown error',
                    detail: response.statusText,
                    code: response.status.toString()
                }));
                throw new Error(error.detail || error.error);
            }
            return response.json();
        });
    }
    checkHealth() {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield fetch(`${this.baseUrl}/health`);
            return this.handleResponse(response);
        });
    }
    getModels() {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield fetch(`${this.baseUrl}/models`);
            const data = yield this.handleResponse(response);
            return data.models;
        });
    }
    askQuestion(request) {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield fetch(`${this.baseUrl}/ask`, {
                method: 'POST',
                headers: this.defaultHeaders,
                body: JSON.stringify(request),
            });
            return this.handleResponse(response);
        });
    }
    explainCode(request) {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield fetch(`${this.baseUrl}/explain`, {
                method: 'POST',
                headers: this.defaultHeaders,
                body: JSON.stringify(request),
            });
            return this.handleResponse(response);
        });
    }
    generateCode(request) {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield fetch(`${this.baseUrl}/generate`, {
                method: 'POST',
                headers: this.defaultHeaders,
                body: JSON.stringify(request),
            });
            return this.handleResponse(response);
        });
    }
    debugCode(request) {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield fetch(`${this.baseUrl}/debug`, {
                method: 'POST',
                headers: this.defaultHeaders,
                body: JSON.stringify(request),
            });
            return this.handleResponse(response);
        });
    }
    analyzeCode(request) {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield fetch(`${this.baseUrl}/analyze`, {
                method: 'POST',
                headers: this.defaultHeaders,
                body: JSON.stringify(request),
            });
            return this.handleResponse(response);
        });
    }
    analyzeFile(file_1) {
        return __awaiter(this, arguments, void 0, function* (file, aiAnalysis = false, model = 'codellama') {
            const formData = new FormData();
            formData.append('file', file);
            formData.append('ai_analysis', aiAnalysis.toString());
            formData.append('model', model);
            const response = yield fetch(`${this.baseUrl}/analyze/file`, {
                method: 'POST',
                body: formData,
            });
            return this.handleResponse(response);
        });
    }
    checkQuality(code_1) {
        return __awaiter(this, arguments, void 0, function* (code, threshold = 70) {
            const response = yield fetch(`${this.baseUrl}/analyze/quality/${threshold}?code=${encodeURIComponent(code)}`);
            return this.handleResponse(response);
        });
    }
    getSupportedLanguages() {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield fetch(`${this.baseUrl}/languages`);
            return this.handleResponse(response);
        });
    }
    getSystemStats() {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield fetch(`${this.baseUrl}/stats`);
            return this.handleResponse(response);
        });
    }
}
export const api = new CodeEducatorAPI();
export const utils = {
    getLanguageFromExtension(filename) {
        var _a;
        const ext = (_a = filename.split('.').pop()) === null || _a === void 0 ? void 0 : _a.toLowerCase();
        const mapping = {
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
    getQualityColor(score) {
        if (score >= 80)
            return '#4CAF50';
        if (score >= 60)
            return '#FFC107';
        if (score >= 40)
            return '#FF9800';
        return '#F44336';
    },
    getQualityGrade(score) {
        if (score >= 90)
            return 'A+';
        if (score >= 80)
            return 'A';
        if (score >= 70)
            return 'B';
        if (score >= 60)
            return 'C';
        if (score >= 50)
            return 'D';
        return 'F';
    },
    getComplexityLevel(complexity) {
        if (complexity <= 5)
            return '매우 간단';
        if (complexity <= 10)
            return '간단';
        if (complexity <= 20)
            return '보통';
        if (complexity <= 40)
            return '복잡';
        return '매우 복잡';
    },
    formatFileSize(bytes) {
        if (bytes === 0)
            return '0 Bytes';
        const k = 1024;
        const sizes = ['Bytes', 'KB', 'MB', 'GB'];
        const i = Math.floor(Math.log(bytes) / Math.log(k));
        return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
    },
    getHighlightClass(language) {
        const mapping = {
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
    askSimpleQuestion() {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                const response = yield api.askQuestion({
                    question: "파이썬에서 리스트와 튜플의 차이점은?",
                    language: "python"
                });
                console.log(response.response);
            }
            catch (error) {
                console.error('Error:', error);
            }
        });
    },
    analyzeCodeExample() {
        return __awaiter(this, void 0, void 0, function* () {
            const code = `
def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)
`;
            try {
                const result = yield api.analyzeCode({
                    code,
                    ai_analysis: true
                });
                console.log('분석 결과:', result);
                console.log('품질 점수:', result.quality_score);
                console.log('잠재적 문제:', result.potential_issues);
            }
            catch (error) {
                console.error('Error:', error);
            }
        });
    },
    analyzeFileExample(file) {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                const result = yield api.analyzeFile(file, true);
                console.log(`${file.name} 분석 완료`);
                console.log('언어:', result.language);
                console.log('품질:', utils.getQualityGrade(result.quality_score));
            }
            catch (error) {
                console.error('Error:', error);
            }
        });
    }
};
// 기본 export
export default CodeEducatorAPI;
