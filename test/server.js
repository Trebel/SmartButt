/* jshint expr: true */
require('chai').should();

var server = require('../lib/server.js');

describe('server.js', function () {

  it('should be ok', function () {
    server.should.be.ok;
  });

});