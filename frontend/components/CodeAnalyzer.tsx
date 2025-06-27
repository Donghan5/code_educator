// frontend/components/CodeAnalyzer.tsx
'use client';

import { useState } from 'react';
import { Editor } from '@monaco-editor/react';
import { Button } from '@/components/ui/button';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';
import { Badge } from '@/components/ui/badge';
import { Alert, AlertDescription } from '@/components/ui/alert';
import { CodeEducatorAPI, AnalysisResult } from '@/lib/api';
import { Upload, Play, AlertTriangle, CheckCircle, Info } from 'lucide-react';

export default function CodeAnalyzer() {
  const [code, setCode] = useState('# 여기에 코드를 입력하세요\ndef hello_world():\n    print("Hello, World!")');
  const [result, setResult] = useState<AnalysisResult | null>(null);
  const [loading, setLoading] = useState(false);
  const [aiAnalysis, setAiAnalysis] = useState(false);

  const handleAnalyze = async () => {
    if (!code.trim()) return;
    
    setLoading(true);
    try {
      const analysisResult = await CodeEducatorAPI.analyzeCode(code, aiAnalysis);
      setResult(analysisResult);
    } catch (error) {
      console.error('분석 실패:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleFileUpload = async (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (!file) return;

    setLoading(true);
    try {
      const analysisResult = await CodeEducatorAPI.analyzeFile(file);
      setResult(analysisResult);
      
      // 파일 내용도 에디터에 표시
      const content = await file.text();
      setCode(content);
    } catch (error) {
      console.error('파일 분석 실패:', error);
    } finally {
      setLoading(false);
    }
  };

  const getQualityColor = (score: number) => {
    if (score >= 80) return 'text-green-600';
    if (score >= 60) return 'text-yellow-600';
    return 'text-red-600';
  };

  return (
    <div className="container mx-auto p-6 space-y-6">
      <div className="text-center">
        <h1 className="text-4xl font-bold mb-2">Code Educator</h1>
        <p className="text-gray-600">AI로 코드를 분석하고 개선점을 찾아보세요</p>
      </div>

      <div className="grid lg:grid-cols-2 gap-6">
        {/* 코드 에디터 */}
        <Card>
          <CardHeader>
            <CardTitle className="flex items-center justify-between">
              코드 입력
              <div className="flex gap-2">
                <label className="cursor-pointer">
                  <input
                    type="file"
                    accept=".py,.js,.cpp,.c,.ts,.tsx"
                    onChange={handleFileUpload}
                    className="hidden"
                  />
                  <Button variant="outline" size="sm">
                    <Upload className="w-4 h-4 mr-2" />
                    파일 업로드
                  </Button>
                </label>
              </div>
            </CardTitle>
          </CardHeader>
          <CardContent>
            <Editor
              height="400px"
              defaultLanguage="python"
              value={code}
              onChange={(value) => setCode(value || '')}
              theme="vs-dark"
              options={{
                minimap: { enabled: false },
                fontSize: 14,
                wordWrap: 'on',
              }}
            />
            <div className="flex items-center gap-4 mt-4">
              <label className="flex items-center gap-2">
                <input
                  type="checkbox"
                  checked={aiAnalysis}
                  onChange={(e) => setAiAnalysis(e.target.checked)}
                />
                AI 분석 포함
              </label>
              <Button onClick={handleAnalyze} disabled={loading}>
                <Play className="w-4 h-4 mr-2" />
                {loading ? '분석 중...' : '코드 분석'}
              </Button>
            </div>
          </CardContent>
        </Card>

        {/* 분석 결과 */}
        <Card>
          <CardHeader>
            <CardTitle>분석 결과</CardTitle>
          </CardHeader>
          <CardContent>
            {!result ? (
              <div className="text-center text-gray-500 py-8">
                코드를 입력하고 분석 버튼을 눌러주세요
              </div>
            ) : (
              <div className="space-y-4">
                {/* 기본 정보 */}
                <div className="grid grid-cols-2 gap-4">
                  <div>
                    <p className="text-sm text-gray-500">언어</p>
                    <Badge variant="secondary">{result.language}</Badge>
                  </div>
                  <div>
                    <p className="text-sm text-gray-500">품질 점수</p>
                    <span className={`text-2xl font-bold ${getQualityColor(result.quality_score)}`}>
                      {result.quality_score}/100
                    </span>
                  </div>
                </div>

                {/* 메트릭스 */}
                <div className="grid grid-cols-2 gap-2 text-sm">
                  <div>라인 수: {result.line_count}</div>
                  <div>복잡도: {result.complexity}</div>
                  <div>주석 비율: {(result.comment_ratio * 100).toFixed(1)}%</div>
                  <div>중첩 깊이: {result.nesting_depth}</div>
                </div>

                {/* 함수/클래스 */}
                {result.functions.length > 0 && (
                  <div>
                    <p className="font-medium mb-2">함수 ({result.functions.length}개)</p>
                    <div className="flex flex-wrap gap-1">
                      {result.functions.slice(0, 5).map((func, i) => (
                        <Badge key={i} variant="outline" className="text-xs">
                          {func}
                        </Badge>
                      ))}
                      {result.functions.length > 5 && (
                        <Badge variant="outline" className="text-xs">
                          +{result.functions.length - 5}개 더
                        </Badge>
                      )}
                    </div>
                  </div>
                )}

                {/* 문제점 */}
                {result.potential_issues.length > 0 && (
                  <Alert>
                    <AlertTriangle className="h-4 w-4" />
                    <AlertDescription>
                      <p className="font-medium mb-2">잠재적 문제점:</p>
                      <ul className="text-sm space-y-1">
                        {result.potential_issues.map((issue, i) => (
                          <li key={i}>• {issue}</li>
                        ))}
                      </ul>
                    </AlertDescription>
                  </Alert>
                )}

                {/* 개선 제안 */}
                {result.suggestions.length > 0 && (
                  <Alert>
                    <CheckCircle className="h-4 w-4" />
                    <AlertDescription>
                      <p className="font-medium mb-2">개선 제안:</p>
                      <ul className="text-sm space-y-1">
                        {result.suggestions.map((suggestion, i) => (
                          <li key={i}>• {suggestion}</li>
                        ))}
                      </ul>
                    </AlertDescription>
                  </Alert>
                )}

                {/* AI 분석 */}
                {result.ai_analysis && (
                  <Alert>
                    <Info className="h-4 w-4" />
                    <AlertDescription>
                      <p className="font-medium mb-2">AI 분석:</p>
                      <div className="text-sm whitespace-pre-wrap">
                        {result.ai_analysis}
                      </div>
                    </AlertDescription>
                  </Alert>
                )}
              </div>
            )}
          </CardContent>
        </Card>
      </div>
    </div>
  );
}