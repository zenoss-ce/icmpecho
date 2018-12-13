#!/usr/bin/env groovy

node {

  stage('Checkout') {
    checkout scm
  }

  stage('Build') {
    sh "make clean"
    sh "[ -f setup.py ] || make setup.py"
    docker.image('zenoss/build-tools:0.0.3').inside() { 
      sh "make build IN_DOCKER="
      sh "make install IN_DOCKER="
    }
  }

  stage('Publish') {
    def remote = [:]
    withFolderProperties {
      withCredentials( [sshUserPrivateKey(credentialsId: 'PUBLISH_SSH_KEY', keyFileVariable: 'identity', passphraseVariable: '', usernameVariable: 'userName')] ) {
        remote.name = env.PUBLISH_SSH_HOST
        remote.host = env.PUBLISH_SSH_HOST
        remote.user = userName
        remote.identityFile = identity
        remote.allowAnyHosts = true

        def tar_ver = sh( returnStdout: true, script: "awk '/^VERSION/{print \$3}' makefile" ).trim()
        sshPut remote: remote, from: 'dist/icmpecho-' + tar_ver + '.tar.gz', into: env.PUBLISH_SSH_DIR
      }
    }
  }

}
