module.exports = function (grunt) {
  // Project Configuration
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    jshint: {
      src: [
        'api_version.js', 'Gruntfile.js', 'bin/server', 'lib/*.js', 'test/*.js'
      ],
      options: {
        jshintrc: true
      }
    },
    mochaTest: {
      src: ['test/*.js'],
      options: {
        reporter: 'spec',
      }
    },
    nodemon: {
      all: {
        script: 'bin/server'
      }
    },
    bunyan: {
      level: 'trace', // set to trace so that no filtering on levels is performed
      output: 'short'
    }
  });

  // Load Plugins and Tasks
  grunt.loadNpmTasks('grunt-contrib-jshint');
  grunt.loadNpmTasks('grunt-mocha-test');
  grunt.loadNpmTasks('grunt-nodemon');
  grunt.loadNpmTasks('grunt-bunyan');

  // Register Custom Tasks
  grunt.registerTask('server', ['lint', 'test', 'bunyan', 'nodemon']);
  grunt.registerTask('default', ['jshint', 'mochaTest']);
  grunt.registerTask('lint', ['jshint']);
  grunt.registerTask('test', ['mochaTest']);
};