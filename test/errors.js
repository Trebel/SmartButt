/* jshint expr: true */
require('chai').should();

var errors = require('../lib/errors.js');

describe('error.js', function () {
  // Initialize Errors
  errors.createErrors();

  it('should be ok', function () {
    errors.should.be.ok;
  });

  it('should provide an error', function () {
    var err = new errors.InternalServerError();

    err.should.be.an.instanceof(Error);
  });

  it('should provide an error which is restify-compatible', function () {
    var err = new errors.InternalServerError();

    err.should.have.ownProperty('name');
    err.should.have.ownProperty('message');
    err.should.have.ownProperty('statusCode');
    err.should.have.ownProperty('body');
    err.body.should.have.ownProperty('code');
    err.body.should.have.ownProperty('message');
  });
});