# srcs/python/cli.py
"""
Code Educator CLI interface
"""
import sys
import os
import json
import click
from .services.ai_service import AIService
from .services.code_service import CodeAnalysisService

@click.group()
@click.version_option(version="0.1.0")
def cli():
    """Code Educator CLI"""
    pass

# AI 서비스와 코드 분석 서비스 인스턴스
ai_service = AIService()
code_service = CodeAnalysisService()

# main ask command
@click.command()
@click.argument('question')
@click.option('--model', '-m', default='codellama', help='AI 모델 선택')
@click.option('--language', '-l', help='프로그래밍 언어 (코딩 질문인 경우)')
@click.option('--context', '-c', help='추가 컨텍스트')
def ask(question, model, language, context):
    """AI에게 프로그래밍 질문하기"""
    try:
        # AI 서비스 상태 확인
        status = ai_service.check_ai_status()
        if not status['connected']:
            click.echo(click.style("Ollama API에 연결할 수 없습니다. 서버를 확인해주세요.", fg='red'))
            sys.exit(1)

        click.echo(click.style(f"질문: ", fg='green') + question)
        click.echo(click.style("답변: ", fg='green'))

        if language:
            response = ai_service.get_coding_help(question, language, model)
        else:
            response = ai_service.ask_question(question, model, context)
            
        click.echo(response)
        
    except Exception as e:
        click.echo(click.style(f"오류: {str(e)}", fg='red'))
        sys.exit(1)

# 코드 설명 명령어
@click.command()
@click.argument('file_path', type=click.Path(exists=True))
@click.option('--language', '-l', help='프로그래밍 언어')
@click.option('--model', '-m', default='codellama', help='AI 모델 선택')
def explain(file_path, language, model):
    """코드 파일 설명"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            code = f.read()
        
        click.echo(click.style(f"파일 설명: {file_path}", fg='blue'))
        click.echo(click.style("설명: ", fg='green'))
        
        response = ai_service.explain_code(code, language, model)
        click.echo(response)
        
    except Exception as e:
        click.echo(click.style(f"오류: {str(e)}", fg='red'))
        sys.exit(1)

# 코드 생성 명령어
@click.command()
@click.argument('description')
@click.argument('language')
@click.option('--model', '-m', default='codellama', help='AI 모델 선택')
@click.option('--output', '-o', help='출력 파일 경로')
def generate(description, language, model, output):
    """설명을 바탕으로 코드 생성"""
    try:
        click.echo(click.style(f"요구사항: {description}", fg='blue'))
        click.echo(click.style(f"언어: {language}", fg='blue'))
        click.echo(click.style("생성된 코드: ", fg='green'))
        
        response = ai_service.generate_code(description, language, model)
        click.echo(response)
        
        if output:
            with open(output, 'w', encoding='utf-8') as f:
                f.write(response)
            click.echo(click.style(f"코드가 {output}에 저장되었습니다.", fg='green'))
        
    except Exception as e:
        click.echo(click.style(f"오류: {str(e)}", fg='red'))
        sys.exit(1)

# 디버깅 도움 명령어
@click.command()
@click.argument('file_path', type=click.Path(exists=True))
@click.option('--error', '-e', help='오류 메시지')
@click.option('--language', '-l', help='프로그래밍 언어')
@click.option('--model', '-m', default='codellama', help='AI 모델 선택')
def debug(file_path, error, language, model):
    """코드 디버깅 도움"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            code = f.read()
        
        click.echo(click.style(f"디버깅 파일: {file_path}", fg='blue'))
        if error:
            click.echo(click.style(f"오류 메시지: {error}", fg='red'))
        click.echo(click.style("디버깅 도움: ", fg='green'))
        
        response = ai_service.debug_help(code, error, language, model)
        click.echo(response)
        
    except Exception as e:
        click.echo(click.style(f"오류: {str(e)}", fg='red'))
        sys.exit(1)

# check the models available
@click.command()
def models():
    """사용 가능한 AI 모델 목록"""
    try:
        models_list = ai_service.get_available_models()

        if not models_list:
            click.echo("사용 가능한 모델이 없습니다.")
            return

        click.echo(click.style("사용 가능한 모델:", fg='green'))
        for model in models_list:
            size = model.get('size', 'Unknown')
            modified = model.get('modified_at', '')
            click.echo(f"• {model['name']} ({size})")
            if modified:
                click.echo(f"  수정: {modified}")

    except Exception as e:
        click.echo(click.style(f"오류: {str(e)}", fg='red'))
        sys.exit(1)

# check the connection and system status
@click.command()
def check():
    """시스템 상태 확인"""
    try:
        # AI 서비스 상태
        ai_status = ai_service.check_ai_status()
        if ai_status['connected']:
            click.echo(click.style("✅ Ollama API 연결됨", fg='green'))
            click.echo(f"   기본 모델: {ai_status['default_model']}")
            click.echo(f"   사용 가능한 모델: {len(ai_status['available_models'])}개")
        else:
            click.echo(click.style("❌ Ollama API 연결 실패", fg='red'))

        # 코드 분석 서비스 상태
        analysis_stats = code_service.get_analysis_stats()
        if analysis_stats['core_module_available']:
            click.echo(click.style("✅ C++ 코어 모듈 로드됨", fg='green'))
        else:
            click.echo(click.style("❌ C++ 코어 모듈 없음 (기본 분석만 가능)", fg='yellow'))
        
        click.echo(f"   지원 언어: {', '.join(analysis_stats['supported_languages'])}")
        
        features = analysis_stats['features']
        click.echo("\n사용 가능한 기능:")
        for feature, available in features.items():
            status = "✅" if available else "❌"
            click.echo(f"  {status} {feature}")
            
    except Exception as e:
        click.echo(click.style(f"오류: {str(e)}", fg='red'))
        sys.exit(1)

# analyze the code
@click.command()
@click.argument('file_path', type=click.Path(exists=True))
@click.option('--format', '-f', type=click.Choice(['text', 'json']), default='text', help='출력 형식')
@click.option('--ai', '-a', is_flag=True, help='AI 분석 포함')
@click.option('--model', '-m', default='codellama', help='AI 모델 (--ai 옵션과 함께 사용)')
def analyze(file_path, format, ai, model):
    """코드 파일 분석"""
    try:
        file_name = os.path.basename(file_path)
        click.echo(click.style(f"분석 중: {file_path}", fg='blue'))

        # 코드 분석 서비스 사용
        result = code_service.analyze_file(file_path, ai, model)

        if format == 'json':
            # JSON 형식으로 출력
            click.echo(json.dumps(result, indent=2, ensure_ascii=False))
        else:
            # 텍스트 형식으로 출력
            from .models.schemas import CLIAnalysisResult
            cli_result = CLIAnalysisResult(**result)
            click.echo(cli_result.to_text_format())
            
    except Exception as e:
        click.echo(click.style(f"오류: {str(e)}", fg='red'))
        sys.exit(1)

# 새로운 명령어: 코드 품질 체크
@click.command()
@click.argument('file_path', type=click.Path(exists=True))
@click.option('--threshold', '-t', default=70, help='품질 점수 임계값 (기본: 70)')
def quality(file_path, threshold):
    """코드 품질 점수 확인 (CI/CD에서 사용 가능)"""
    try:
        result = code_service.analyze_file(file_path, False)
        score = result['quality_score']
        
        if score >= threshold:
            click.echo(click.style(f"✅ 품질 점수: {score}/100 (통과)", fg='green'))
            sys.exit(0)
        else:
            click.echo(click.style(f"❌ 품질 점수: {score}/100 (실패, 임계값: {threshold})", fg='red'))
            if result['potential_issues']:
                click.echo("\n문제점:")
                for issue in result['potential_issues']:
                    click.echo(f"  • {issue}")
            sys.exit(1)
            
    except Exception as e:
        click.echo(click.style(f"오류: {str(e)}", fg='red'))
        sys.exit(1)

# register commands
cli.add_command(ask)
cli.add_command(explain)
cli.add_command(generate)
cli.add_command(debug)
cli.add_command(models)
cli.add_command(check)
cli.add_command(analyze)
cli.add_command(quality)

if __name__ == '__main__':
    cli()