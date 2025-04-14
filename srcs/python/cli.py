"""
Code Educator CLI interface
"""
import sys
import os
import json
import click
from .api import OllamaAPI

# C++ 모듈 가져오기
try:
    import code_educator_core as ce
    HAS_CORE = True
except ImportError:
    HAS_CORE = False
    click.echo(click.style("Warning: code_educator_core module not found. Some features will be disabled.", fg='yellow'))

@click.group()
@click.version_option(version="0.1.0")
def cli():
    """Code Educator CLI"""
    pass


# main ask command
@click.command()
@click.argument('question')
@click.option('--model', '-m', default='codellama', help='AI mode to use')
@click.option('--stream/--no-stream', default=True, help='Respon stream acitvate/unactivate')
def ask(question, model, stream):
    """Question programmatically"""
    try:
        api = OllamaAPI(model=model)

        if not api.check_connection():
            click.echo(click.style("Can't connect to Ollama API. Please check it.", fg='red'))
            sys.exit(1)

        click.echo(click.style(f"Request: ", fg='green') + question)
        click.echo(click.style("Response: ", fg='green'))

        if stream:
            # Streaming mode
            response_received = False
            for chunk in api.generate(question, stream=True):
                if chunk:
                    response_received = True
                    if chunk == '\n':
                        click.echo('', nl=True)
                    else:
                        click.echo(chunk, nl=False)
            if not response_received:
                click.echo(click.style("No response generated.", fg='red'))
            else:
                click.echo()
        else:
            # Not Streaming mode
            response = api.generate(question, stream=False)
            if response:
              click.echo(response)
            else:
               click.echo(click.style("No response generated.", fg='red'))
    except Exception as e:
        click.echo(click.style(f"error: {str(e)}", fg='red'))
        sys.exit(1)


# check the models available
@click.command()
def models():
    """List available models"""
    try:
        api = OllamaAPI()
        model_list = api.list_models()

        if not model_list:
            click.echo("NO MODEL AVAILABLE.")
            return

        click.echo(click.style("Model available:", fg='green'))
        for model in model_list:
            size = model.get('size', 'Unknown')
            click.echo(f"• {model['name']} ({size})")

    except Exception as e:
        click.echo(click.style(f"Error: {str(e)}", fg='red'))
        sys.exit(1)

# check the connection to the Ollama API
@click.command()
def check():
    """Connection check Ollama API"""
    try:
        api = OllamaAPI()
        if api.check_connection():
            click.echo(click.style("✅ Ollama API Connected.", fg='green'))
        else:
            click.echo(click.style("❌ Cannot connect to Ollama API.", fg='red'))
            sys.exit(1)

        # 코어 모듈 확인
        if HAS_CORE:
            click.echo(click.style(f"✅ Code module loaded (버전: {ce.version()})", fg='green'))
        else:
            click.echo(click.style("❌ Can not load.", fg='yellow'))
    except Exception as e:
        click.echo(click.style(f"Error: {str(e)}", fg='red'))
        sys.exit(1)

# analyze the code
@click.command()
@click.argument('file_path', type=click.Path(exists=True))
@click.option('--format', '-f', type=click.Choice(['text', 'json']), default='text', help='Output format')
@click.option('--ai', '-a', is_flag=True, help='Adding AI analysis')
@click.option('--model', '-m', default='codellama', help='Using AI model (--ai option)')
def analyze(file_path, format, ai, model):
    """Code file analysis"""
    try:
        # 파일 읽기
        with open(file_path, 'r', encoding='utf-8') as f:
            code = f.read()

        # 파일명과 확장자 추출
        file_name = os.path.basename(file_path)
        file_ext = os.path.splitext(file_name)[1].lower()

        # C++ 코어 모듈 분석
        if HAS_CORE:
            click.echo(click.style(f"Anlyasis Code: {file_path}", fg='blue'))

            # code parserr
            parser = ce.CodeParser()
            structure = parser.parse(code)

            # code analyzer
            analyzer = ce.Analyzer()
            analysis = analyzer.analyze(code)

            # calculate quality score
            quality_score = analyzer.calculate_quality_score(analysis)

            # print result
            if format == 'json':
                # print json format
                output = {
                    'file_path': file_path,
                    'language': structure.language,
                    'complexity': structure.complexity,
                    'imports': list(structure.imports),
                    'functions': list(structure.functions),
                    'classes': list(structure.classes),
                    'line_count': analysis.line_count,
                    'comment_count': analysis.comment_count,
                    'comment_ratio': analysis.comment_ratio,
                    'nesting_depth': analysis.nesting_depth,
                    'cyclomatic_complexity': analysis.cyclomatic_complexity,
                    'potential_issues': list(analysis.potential_issues),
                    'suggestions': list(analysis.suggestions),
                    'quality_score': quality_score
                }
                click.echo(json.dumps(output, indent=2))
            else:
                # 텍스트 형식으로 출력
                click.echo(click.style(f"\nAnalysis code structure: {file_name}", fg='green'))
                click.echo(click.style("Basic information:", fg='blue'))
                click.echo(f"• Language: {structure.language}")
                click.echo(f"• Complexity: {structure.complexity}")
                click.echo(f"• Number of line: {analysis.line_count}")
                click.echo(f"• Number of comment: {analysis.comment_count} (ratio: {analysis.comment_ratio:.2f})")

                click.echo(click.style("\nComplexity of Code:", fg='blue'))
                click.echo(f"• Nesting Depth: {analysis.nesting_depth}")
                click.echo(f"• Cyclomatic complexity: {analysis.cyclomatic_complexity}")

                # 품질 점수 색상 설정
                if quality_score >= 80:
                    score_color = 'green'
                elif quality_score >= 60:
                    score_color = 'yellow'
                else:
                    score_color = 'red'
                click.echo(click.style(f"• Score code quality: {quality_score}/100", fg=score_color))

                if structure.imports:
                    click.echo(click.style("\nimport/include:", fg='blue'))
                    for imp in structure.imports:
                        click.echo(f"• {imp}")

                if structure.functions:
                    click.echo(click.style("\nFunction:", fg='blue'))
                    for func in structure.functions:
                        click.echo(f"• {func}")

                if structure.classes:
                    click.echo(click.style("\nClass/Structure:", fg='blue'))
                    for cls in structure.classes:
                        click.echo(f"• {cls}")

                if analysis.potential_issues:
                    click.echo(click.style("\nPotential problems:", fg='yellow'))
                    for issue in analysis.potential_issues:
                        click.echo(f"• {issue}")

                if analysis.suggestions:
                    click.echo(click.style("\nSuggestion improvement:", fg='green'))
                    for suggestion in analysis.suggestions:
                        click.echo(f"• {suggestion}")
        else:
            click.echo(click.style("Do not import core module, excute the basic anylsis.", fg='yellow'))

            # 기본 분석 (라인 수, 파일 크기 등)
            lines = code.splitlines()
            line_count = len(lines)
            file_size = os.path.getsize(file_path)

            if format == 'json':
                output = {
                    'file_path': file_path,
                    'file_size': file_size,
                    'line_count': line_count
                }
                click.echo(json.dumps(output, indent=2))
            else:
                click.echo(click.style(f"\nDefault file analysis: {file_name}", fg='green'))
                click.echo(f"• Size of file: {file_size} bytes")
                click.echo(f"• Number of line: {line_count}")

        # AI를 사용한 추가 분석
        if ai:
            click.echo(click.style("\nAdditional anlysis with AI", fg='blue'))
            api = OllamaAPI(model=model)

            if not api.check_connection():
                click.echo(click.style("Do not connect with API Ollama.", fg='red'))
                return

            # AI 프롬프트 생성
            prompt = f"""
            Next {structure.language if HAS_CORE else file_ext[1:]} anlysis,
            Evaluate the code quality and provide suggestions for improvement.
            Please explain the code and provide suggestions for improvement.

            ```{structure.language if HAS_CORE else file_ext[1:]}
            {code[:4000] if len(code) > 4000 else code}
            ```

            {f"참고: This code has more {len(code) - 4000} characters but it cutted due to limits of length" if len(code) > 4000 else ""}
            """

            # AI 응답 생성
            click.echo(click.style("\nResult of AI analysis:", fg='green'))
            response = api.generate(prompt, stream=False)
            click.echo(response)

    except Exception as e:
        click.echo(click.style(f"Error: {str(e)}", fg='red'))
        sys.exit(1)


# register command
cli.add_command(ask)
cli.add_command(models)
cli.add_command(check)
cli.add_command(analyze)

if __name__ == '__main__':
    cli()
