import { api, utils } from './api-client';

// '!' at the end it means that the element is not null
const analyzeBtn = document.getElementById('analyze-button')!;
const codeInput = document.getElementById('code-input')!;
const aiCheck = document.getElementById('ai-analysis')!;

analyzeBtn.addEventListener('click', async () => {
    const code = codeInput.value;
    const ai = aiCheck.checked;
    const model = document.getElementById('model-select').value;
})