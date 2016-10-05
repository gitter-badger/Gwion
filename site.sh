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
	bundle exec jekyll b --config _config.yml,_config_deploy.yml
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
	bundle exec jekyll s --config _config.yml
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
	echo "find the examples modules"
	echo "'cd' in int"
	echo "iterative loop trough '.gw' files in dir"
	echo "make headers"
	echo -e "---\nlayout:example\,title:'example name'\n---"
	pygmentize -f html /tmp/file.gw
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
