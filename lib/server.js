function start(config) {
  var restify = require('restify');
  var logger = require('./logger.js');
  var os = require('os');
  var path = require('path');
  var errors = require('./errors.js');
  var util = require('util');

  // Statics
  const APPLICATION_ROOT = path.resolve(__dirname + '/..');
  const API_VERSION = require('../api_version');

  // Exports
  module.exports.APPLICATION_ROOT = APPLICATION_ROOT;
  module.exports.hostname = os.hostname();

  // Read config
  config = config ? require(config) : require('../conf/server.json');
  module.exports.config = config;
  module.exports.port = config.port;

  // Set the process' name
  process.title = config.name;
  module.exports.title = config.name;

  // Set up errors
  errors.createErrors();

  // Set up logger
  var log = logger.createLogger(config.log, process.title);
  module.exports.log = log;

  // Create Restify Server
  var server = restify.createServer({
    name: process.title,
    version: API_VERSION,
    log: log
  });

  // Set up mdws
  var serverMdws = [
    // Restify mdws
    restify.requestLogger(),
    restify.fullResponse(),
    restify.acceptParser(server.acceptable),
    restify.dateParser(config.allowedClockSkewSeconds),
    restify.queryParser(),
    restify.bodyParser({ mapParams: false }),
    // Custom mdws
    function (req, res, next) {
      var message = util.format('[%s] [%s] [%s]', req.method, req.href(), req.version());
      req.log.info({req: req}, message);
      next();
    }
  ];
  serverMdws.forEach(function (mdw) {
    server.use(mdw);
  });

  // Log outgoing requests
  server.on('after', function (req, res) {
    var message = util.format('[%s] [%s] [%s] [%s]',
      req.method, req.href(), res.statusCode, req.version());

    if (res.statusCode >= 200 && res.statusCode < 500)
      req.log.info({res: res, req: req}, message);
    else
      req.log.error({res: res, req: req}, message);
  });

  // Log any client or server errors
  server.on('error', function (err) {
    log.error(err.stack, '[server error] ~ ' + err.name + ' ~ ' + err.message);
  });
  server.on('clientError', function (err) {
    log.error(err.stack, '[client error] ~ ' + err.name + ' ~ ' + err.message);
  });

  // Handle uncaught exceptions
  server.on('uncaughtException', function (req, res, route, err) {
    req.log.fatal(err.stack, '[uncaught exception] ~ ' + route.name + ' ~ ' + err.message);
    if (!res.finished) {
      var e = new errors.UncaughtExceptionError(err.message);
      res.json(500, e.body);
      res.end();
    }
    err.handled = true;
  });
  process.on('uncaughtException', function (err) {
    if (err.handled) return;
    log.fatal(err.stack, '[uncaught exception] ~ ' + err.name + ' ~ ' + err.message);
  });

  // Load routes. Config must be loaded first.
  var routes = require('./routes');
  routes.loadRoutes(server, log);

  // Start listening on port
  server.listen(config.port, function () {
    log.info(server.name + ' listening on ' + server.url);
    server.emit('listening', true);
  });

  return server;
}
module.exports.start = start;

/**
 * console.dir is just a wrapper around util.inspect using defaults
 * for colourisation and depth.
 *
 * Replace it with our own wrapper with much deeper inspection and
 * colors on the console.
 *
 * It's just for development since no console.dir calls should be
 * left in production code anyway.
 */
(function () {
  var util = require('util');
  console.dir = function dbg(obj, depth) {
    console.log(util.inspect(obj, false, depth || 10, true));
  };
})();