node {
    // Git checkout before load source the file
    checkout scm

    // To know files are checked out or not
    sh '''
        ls -lhrt
    '''

    def rootDir = pwd()
    println("Current Directory: " + rootDir)

    // point to exact source file
    def example = load "${rootDir}/Example.Groovy"

    example.exampleMethod()
    example.otherExampleMethod()
}
