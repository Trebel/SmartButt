/* jshint expr: true */
require('chai').should();

var logger = require('../lib/logger.js');

describe('logger.js', function () {

  it('should be ok', function () {
    logger.should.be.ok;
  });

  it('should provide a logger when given a valid config', function () {
    var loggerConfig = {
      console: {
        enabled: true,
        level: 'debug'
      }
    };
    var log = logger.createLogger(loggerConfig, 'logger');
    log.should.be.ok;
  });

  it('should provide a logger if no config is provided', function () {
    var loggerConfig = {};
    var log = logger.createLogger(loggerConfig, 'logger');
    log.should.be.ok;
  });
});