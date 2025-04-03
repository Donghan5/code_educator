"""
Code Educator CLI interface
"""
import sys
import click
from .api import OllamaAPI

@click.group()
@click.version_option(version="0.1.0")
def cli():
    """Code Educator CLI"""
    pass


@cli.command()
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

        click.echo(click.style(f"질문: ", fg='green') + question)
        click.echo(click.style("응답: ", fg='green'))

        if stream:
            # Streaming mode
            for chunk in api.generate(question, stream=True):
                click.echo(chunk, nl=False)
            click.echo()  # chenage line after streaming
        else:
            # Not Streaming mode
            click.echo(api.generate(question, stream=False))

    except Exception as e:
        click.echo(click.style(f"오류: {str(e)}", fg='red'))
        sys.exit(1)


@cli.command()
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


@cli.command()
def check():
    """Connection check Ollama API"""
    try:
        api = OllamaAPI()
        if api.check_connection():
            click.echo(click.style("✅ Ollama API Connected.", fg='green'))
        else:
            click.echo(click.style("❌ Cannot connect to Ollama API.", fg='red'))
            sys.exit(1)
    except Exception as e:
        click.echo(click.style(f"Error: {str(e)}", fg='red'))
        sys.exit(1)


if __name__ == '__main__':
    cli()
