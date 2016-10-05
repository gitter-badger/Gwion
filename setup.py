from setuptools import setup, find_packages

setup (
  name='gwionlexer',
  packages=find_packages(),
  entry_points =
  """
  [pygments.lexers]
  gwionlexer = gwionlexer.lexer:GwionLexer
  """,
)
