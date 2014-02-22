var bunyan = require('bunyan');
var bsyslog = require('bunyan-syslog');

function createLogger(loggerConfig, loggerName) {
  var streams = [];

  // Setup console logging
  if (loggerConfig.console && loggerConfig.console.enabled === true) {
    var consoleConfig = {
      level: loggerConfig.console.level || 'info',
      stream: process.stdout
    };
    streams.push(consoleConfig);
  }

  // Setup file logging
  if (loggerConfig.file && loggerConfig.file.enabled === true) {
    var fileConfig = {
      level: loggerConfig.file.level || 'info',
      path: loggerConfig.file.path || require('./server.js').APPLICATION_ROOT + '/log/log.log',
    };
    streams.push(fileConfig);
  }

  // Setup syslog logging
  if (loggerConfig.syslog && loggerConfig.syslog.enabled === true) {
    var syslogConfig = {
      level: loggerConfig.syslog.level,
      type: 'raw',
      stream: bsyslog.createBunyanStream({
        type: loggerConfig.syslog.type ? loggerConfig.syslog.type : undefined,
        facility: loggerConfig.syslog.facility ? loggerConfig.syslog.facility : undefined,
        host: loggerConfig.syslog.host ? loggerConfig.syslog.host : undefined,
        port: loggerConfig.syslog.port ? loggerConfig.syslog.port : undefined
      })
    };
    streams.push(syslogConfig);
  }

  // Create logger
  var config = {
    name: loggerName,
    serializers: bunyan.stdSerializers,
    streams: streams
  };

  var logger = bunyan.createLogger(config);
  return logger;
}
module.exports.createLogger = createLogger;