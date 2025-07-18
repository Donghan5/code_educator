import { api, utils, type AnalysisResult } from './api-client.js';

class CodeEducatorApp {
  private analyzeBtn!: HTMLButtonElement;
  private codeInput!: HTMLTextAreaElement;
  private resultContainer!: HTMLDivElement;
  private loadingSpinner!: HTMLDivElement;

  constructor() {
    this.initElements();
    this.initEventListeners();
  }

  private initElements() {
    this.analyzeBtn      = document.getElementById('analyze-button')  as HTMLButtonElement;
    this.codeInput       = document.getElementById('code-input')      as HTMLTextAreaElement;
    this.resultContainer = document.getElementById('analysis-results') as HTMLDivElement;

    this.loadingSpinner          = document.createElement('div');
    this.loadingSpinner.id       = 'loading-spinner';
    this.loadingSpinner.textContent = '분석 중...';
    this.loadingSpinner.style.display = 'none';

    this.resultContainer.parentElement?.insertBefore(
      this.loadingSpinner,
      this.resultContainer
    );
  }

  private initEventListeners() {
    this.analyzeBtn.addEventListener('click', () => this.analyzeCode());
  }

  private async analyzeCode() {
    const code = this.codeInput.value.trim();
    if (!code) return;

    try {
      this.showLoading();

      const result = await api.analyzeCode({
        code,
        ai_analysis: true
      });

      this.displayResults(result);
    } catch (err) {
      this.showError((err as Error).message);
    } finally {
      this.hideLoading();
    }
  }

  private displayResults(result: AnalysisResult) {
    this.resultContainer.innerHTML = '';

    const qualityColor = utils.getQualityColor(result.quality_score);
    const grade        = utils.getQualityGrade(result.quality_score);

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

    if (result.potential_issues?.length) {
      this.resultContainer.appendChild(this.createHeading('잠재적 문제'));
      this.resultContainer.appendChild(this.createList(result.potential_issues));
    }

    if (result.suggestions?.length) {
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

  private createHeading(text: string): HTMLHeadingElement {
    const h4 = document.createElement('h4');
    h4.textContent = text;
    return h4;
  }

  private createList(items: string[]): HTMLUListElement {
    const ul = document.createElement('ul');
    items.forEach(i => {
      const li = document.createElement('li');
      li.textContent = i;
      ul.appendChild(li);
    });
    return ul;
  }

  private showLoading() {
    this.loadingSpinner.style.display = 'block';
    this.resultContainer.innerHTML = '';
  }

  private hideLoading() {
    this.loadingSpinner.style.display = 'none';
  }

  private showError(message: string) {
    this.resultContainer.innerHTML =
      `<p style="color:red;">오류: ${message}</p>`;
  }
}

document.addEventListener('DOMContentLoaded', () => new CodeEducatorApp());
