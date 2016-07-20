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

# Releasing
Use git flow to release a version to the `master` branch. A jenkins job can be triggered manually to build and publish the
*tar* artifact to zenpip.  During the git flow release process, update the version in the makefile by removing the `dev`
suffix and then increment the version number in the `develop` branch.

## Versioning

The version convention for this artifact is `icmpecho-<version>.tar.gz` where `<version>`
is the version of icmpecho

By convention, the `develop` branch should have the next revision number, a number higher than what is
currently released, with the `-dev` suffix and the `master` branch will have the currently released version.
For example, if the currently released version is `1.0.1` on master, then
the version in the `develop` should be `1.0.2-dev`.

## Release Steps

1. Check out the `master` branch and make sure to have latest `master`.
  * `git checkout master`
  * `git pull origin master`

2. Check out the `develop` branch.
  * `git checkout develop`
  * `git pull origin develop`

3. Start release of next version. The version is usually the version in the makefile minus the `-dev` suffix.  e.g., if the version
  in `develop` is `1.0.2-dev` and in `master` `1.0.1`, then the
  `<release_name>` will be the new version in `master`, i.e. `1.0.2`.
  *  `git flow release start <release_name>`

4. Update the `VERSION` variable in the `makefile` file. e.g set it to `1.0.2`

5. run `make` to make sure everything builds properly.

6. Commit and tag everything, don't push.
  * `git commit....`
  * `git flow release finish <release_name>`
  * `git push origin --tags`

7. You will be on the `develop` branch again. While on `develop` branch, edit the the `VERSION` variable in the `makefile` file to
be the next development version. For example, if you just released version 1.0.2, then change the `VERSION` variable to
`1.0.2-dev`.

8. Check in `develop` version bump and push.
  * `git commit...`
  * `git push`

9. Push the `master` branch which should have the new released version.
  * `git checkout master`
  * `git push`

10. Have someone manually kick off the jenkins job to build master which will publish the artifact to zenpip.


