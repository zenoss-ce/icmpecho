# icmpecho
This repo defines the `icmpecho` artifact

# Building
To buid a dev artifact for testing locally, use
  * `git checkout develop`
  * `git pull origin devlop`
  * `make clean build`

The result should be two files in the `dist` subdirectory - a wheel containing the python package,
and a binary executable named `pyraw`.
If you need to make changes, create a feature branch like you would for any other kind of change, modify the
code as necessary, use `make clean build` to rebuild the artifacts and then test it as necessary.

Once you have finished your local testing, commit your changes, push them, and create a pull-request as you would
normally. A Jenkins PR build will be started to verify that your changes will build in
a Jenkins environment.

## Releasing

Use git flow to release a version to the `master` branch.

The artifact version number is defined in the [makefile](./makefile).

For Zenoss employees, the details on using git-flow to release a new version is documented on the Zenoss Engineering 
web site [here](https://sites.google.com/a/zenoss.com/engineering/home/faq/developer-patterns/using-git-flow).
After the git flow release process is complete, a jenkins job must be triggered manually to build and publish the artifact. 
