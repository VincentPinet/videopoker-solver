pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                sh 'cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1'
                sh 'cmake --build build -j'
            }
        }
        stage('Test') {
            steps {
                sh 'ctest -VV --test-dir build'
            }
        }
        stage('Analysis') {
            steps {
                sh 'cppcheck --enable=all --suppress=missingIncludeSystem --suppress=checkersReport --xml --output-file=cppcheck-result.xml --project=build/compile_commands.json'
                publishCppcheck()
                xunit(tools: [ GoogleTest(pattern: '**/gtest-result/*.xml') ])
                sh 'gcovr --cobertura > cobertura.xml'
                recordCoverage(tools: [[parser: 'COBERTURA']], sourceCodeRetention: 'EVERY_BUILD')
            }
        }
    }
}
