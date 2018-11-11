#!/usr/bin/env groovy

node {

  stage('Checkout') {
    checkout scm
  }

  stage('Build') {
    sh "[ -f setup.py ] || make setup.py"
    docker.image('zenoss/build-tools:0.0.3').inside() { 
      sh "make build IN_DOCKER="
      sh "make install IN_DOCKER="
    }
  }

  stage('Install') {
    println('Install stage')
  }

}
