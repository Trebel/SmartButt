var M2X = require('m2x');
var config = require('./server.js').config;

function m2xClient(apiKey) {
  return function (req, res, next) {
    req.m2x = new M2X(apiKey);
    return next();
  };
}
module.exports.m2xClientMdw = m2xClient(config.m2xMasterKey);