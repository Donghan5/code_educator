var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
import { api, utils } from './api-client.js';
class CodeEducatorApp {
    constructor() {
        this.initElements();
        this.initEventListeners();
    }
    initElements() {
        var _a;
        this.analyzeBtn = document.getElementById('analyze-button');
        this.codeInput = document.getElementById('code-input');
        this.resultContainer = document.getElementById('analysis-results');
        this.loadingSpinner = document.createElement('div');
        this.loadingSpinner.id = 'loading-spinner';
        this.loadingSpinner.textContent = '분석 중...';
        this.loadingSpinner.style.display = 'none';
        (_a = this.resultContainer.parentElement) === null || _a === void 0 ? void 0 : _a.insertBefore(this.loadingSpinner, this.resultContainer);
    }
    initEventListeners() {
        this.analyzeBtn.addEventListener('click', () => this.analyzeCode());
    }
    analyzeCode() {
        return __awaiter(this, void 0, void 0, function* () {
            const code = this.codeInput.value.trim();
            if (!code)
                return;
            try {
                this.showLoading();
                const result = yield api.analyzeCode({
                    code,
                    ai_analysis: true
                });
                this.displayResults(result);
            }
            catch (err) {
                this.showError(err.message);
            }
            finally {
                this.hideLoading();
            }
        });
    }
    displayResults(result) {
        var _a, _b;
        this.resultContainer.innerHTML = '';
        const qualityColor = utils.getQualityColor(result.quality_score);
        const grade = utils.getQualityGrade(result.quality_score);
        const summary = document.createElement('div');
        summary.innerHTML = `
      <h3>품질 점수: <span style="color:${qualityColor}; font-weight:bold;">
        ${result.quality_score} (${grade})
      </span></h3>
      <p>언어: ${result.language}</p>
      <p>라인 수: ${result.line_count}, 주석 비율: ${(result.comment_ratio * 100).toFixed(1)}%</p>
      <p>복잡도: ${utils.getComplexityLevel(result.cyclomatic_complexity)}
         (CC ${result.cyclomatic_complexity})</p>
    `;
        this.resultContainer.appendChild(summary);
        if ((_a = result.potential_issues) === null || _a === void 0 ? void 0 : _a.length) {
            this.resultContainer.appendChild(this.createHeading('잠재적 문제'));
            this.resultContainer.appendChild(this.createList(result.potential_issues));
        }
        if ((_b = result.suggestions) === null || _b === void 0 ? void 0 : _b.length) {
            this.resultContainer.appendChild(this.createHeading('개선 제안'));
            this.resultContainer.appendChild(this.createList(result.suggestions));
        }
        if (result.ai_analysis) {
            this.resultContainer.appendChild(this.createHeading('AI 분석'));
            const pre = document.createElement('pre');
            pre.textContent = result.ai_analysis;
            pre.style.whiteSpace = 'pre-wrap';
            this.resultContainer.appendChild(pre);
        }
    }
    createHeading(text) {
        const h4 = document.createElement('h4');
        h4.textContent = text;
        return h4;
    }
    createList(items) {
        const ul = document.createElement('ul');
        items.forEach(i => {
            const li = document.createElement('li');
            li.textContent = i;
            ul.appendChild(li);
        });
        return ul;
    }
    showLoading() {
        this.loadingSpinner.style.display = 'block';
        this.resultContainer.innerHTML = '';
    }
    hideLoading() {
        this.loadingSpinner.style.display = 'none';
    }
    showError(message) {
        this.resultContainer.innerHTML =
            `<p style="color:red;">오류: ${message}</p>`;
    }
}
document.addEventListener('DOMContentLoaded', () => new CodeEducatorApp());
