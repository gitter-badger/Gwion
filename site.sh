#!/bin/sh

function deploy()
{
	git clone -b gh-pages `git config remote.origin.url` _site 2> /dev/null
	mv README.md  README.old
	echo 'built by site/mk_size.sh' > README.md
	echo '_site' > .gitignore
	bundle exec jekyll b --config _config.yml,_config_deploy.yml -q
	cd _site
	git add -A
	git commit -am 'Yeah. Built from subdir'
	git push
	cd ..
	rm -rf _site
	mv README.old README.md
	rm Gemfile.lock
	git add -A	> /dev/null
	git commit -am 'Yeah. Built from subdir' 2> /dev/null
	git push > /dev/null
}

function run()
{
	bundle exec jekyll s -q
	rm -rf _site
}

if("$1" == "run")
then
	run 1
elif("$1" == "deploy")
then
	./deploy 1
else
	echo "$0 needs 'run' or 'deploy' as argument"
fi
