#!/bin/sh

# deploy site on gh-pages
function deploy()
{
	git clone -b gh-pages `git config remote.origin.url` _site
	cd _site
	git rm -r *
	git commit -m "auto clean"
	git push
	cd ..
	mv README.md  README.old
	echo 'built by site/mk_size.sh' > README.md
	echo '_site' > .gitignore
	bundle exec jekyll b
	cd _site
	git add -A
	git commit -am 'Yeah. Built from subdir'
	git push
	cd ..
	rm -rf _site
	mv README.old README.md
	rm Gemfile.lock
	git add -A
	git commit -am 'Yeah. Built from subdir'
	git push > /dev/null
}

#run the size locally. access with localhost:4000
function run()
{
	bundle exec jekyll s
	rm -rf _site
}

# remove _site if interupted or ...
trap 'rm -rf _site; exit' INT
trap 'rm -rf _site; exit' QUIT
trap 'rm -rf _site; exit' TERM
trap 'rm -rf _site; exit' EXIT

function send_gist()
{
	echo $(gist -u $1 -f $2| sed "s/https:\/\/gist.github.com\///") >> known_gist
}

function examples()
{
	rm -rf examples Gwion-examples
	git checkout master -- examples
	mv examples Gwion-examples
	mkdir examples
	for ex in $(ls Gwion-examples/*.gw)
	do
		NAME=$(basename $ex .gw)
		echo -e "---\nlayout: example\ntitle: example $NAME\ncategories: examples \n---\n<br>this page documents <b>$NAME.gw</b><br><p>" > examples/$NAME.html
		pygmentize -f html $ex >> examples/$NAME.html
	echo "</p>" >> examples/$NAME.html
	done
	echo '---
layout: homepage
title:	{{post.title}}
---
  <h1>Examples</h1>
  <ul class="posts">
{% for post in site.pages %}
	{% if post.categories == "examples" %}
		<li><span><a href="{{ site.baseurl }}{{ post.url }}">{{post.title}}</a></li>
	{% endif %}
{% endfor %}
  </ul>' > examples/index.html


	rm -r Gwion-examples
	echo "put example content, whith highligting"
	echo "you should be done."
}

function code()
{
	rm -f /tmp/file.gw
	nano /tmp/file.gw
	pygmentize -f html /tmp/file.gw | xclip -i
	echo "should be"
	pygmentize /tmp/file.gw
	rm -f /tmp/file.gw
}

# parse arguments
echo $1
if [ "${1}" = 'run' ]
then
	run
elif [ "${1}" = 'gist' ]
then
	send_gist $2 $3
elif [ "${1}" = 'deploy' ]
then
	deploy
elif [ "${1}" = 'code' ]
then
	code
elif [ "${1}" = 'examples' ]
then
#	echo "$1:not ready yet."
#	exit 1
	examples
else
	echo "$0 needs 'run' or 'deploy' as argument"
fi
