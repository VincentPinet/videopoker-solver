pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                sh 'rm -rf build'
                sh 'cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1'
                sh 'cmake --build build -j'
            }
        }
        stage('Test') {
            steps {
                sh 'make -C build test ARGS="-V"'
            }
        }
        stage('Analysis') {
            steps {
                sh 'cppcheck --enable=all --suppress=missingIncludeSystem --xml --output-file=cppcheck-result.xml --project=build/compile_commands.json'
                publishCppcheck()
                xunit(tools: [ GoogleTest(pattern: '**/gtest-result/*.xml') ])
                sh 'gcovr --cobertura > coverage-result.xml'
                cobertura(coberturaReportFile: 'coverage-result.xml')
            }
        }
    }
}
