var restify = require('restify');

var server = require('../server.js');

module.exports.handler = restify.serveStatic({
  directory: server.APPLICATION_ROOT + '/assets/app'
});