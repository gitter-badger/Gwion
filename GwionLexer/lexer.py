from pygments.lexer import CppLexer
from pygments.token import Punctuation, Text, Keyword, Name, String, Number
#from pygments.util import shebang_matches
class GwionLexer(CppLexer):
	name = 'Gwion'
	aliases = 'gwion'
	filenames = ['gw']
	tokens = {
		'root': [
		(r'"[^"]+"', String),
		(r'""".+"""', Text),
		(r'\b(needs|includes|requires|when|fail|is|a|the)\s*\b', Keyword),
		(r'([A-Z][a-z]+)+', Name),
		(r'[,;:]', Punctuation),
		],
	}
	def analyse_text(text):
		return shebang_matches(text, r'requs')
