var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
import axios from 'axios';
const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8000';
const api = axios.create({
    baseURL: API_BASE_URL,
    headers: {
        'Content-Type': 'application/json',
    },
});
export class CodeEducatorAPI {
    static healthCheck() {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield api.get('/health');
            return response.data;
        });
    }
    static getModels() {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield api.get('/models');
            return response.data.models;
        });
    }
    static askQuestion(question_1) {
        return __awaiter(this, arguments, void 0, function* (question, model = 'codellama') {
            const response = yield api.post('/ask', {
                question,
                model,
                stream: false,
            });
            return response.data.response;
        });
    }
    static analyzeCode(code_1) {
        return __awaiter(this, arguments, void 0, function* (code, aiAnalysis = false, model = 'codellama') {
            const response = yield api.post('/analyze', {
                code,
                ai_analysis: aiAnalysis,
                model,
            });
            return response.data;
        });
    }
    static analyzeFile(file) {
        return __awaiter(this, void 0, void 0, function* () {
            const formData = new FormData();
            formData.append('file', file);
            const response = yield api.post('/analyze/file', formData, {
                headers: {
                    'Content-Type': 'multipart/form-data',
                },
            });
            return response.data;
        });
    }
}
export default CodeEducatorAPI;
