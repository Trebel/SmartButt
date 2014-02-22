var async = require('async');
var errors = require('../errors.js');
var deepMixIn = require('mout/object/deepMixIn');

function getStatus(req, res, next) {
  async.auto({
    m2xStatus: function (callback) {
      req.m2x.status(function (status) {
        if (status.status === 'OK')
          return callback(null, {m2x: true});
        else
          return callback(null, {m2x: false});
      });
    }
  }, function (err, results) {
    if (err instanceof Error)
      return next(errors.getBestError(err.statusCode, err.message));

    res.json(200, deepMixIn({}, results.m2xStatus));
    return next();
  });
}
module.exports.handler = getStatus;