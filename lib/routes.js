var mdw = require('./mdw.js');

const routes = [
  {
    method: 'get',
    path: '/status',
    version: undefined,
    before: [mdw.m2xClientMdw],
    handler: require('./routes/get_status.js').handler
  }
];

function pushToChain(chain, routes) {
  if (Array.isArray(routes))
    chain.push.apply(chain, routes);
  else if (routes instanceof Function)
    chain.push(routes);
}

function loadRoutes(server, log) {
  var chain;
  routes.forEach(function (route) {
    chain = [];
    if (route.before) pushToChain(chain, route.before);
    pushToChain(chain, route.handler);

    server[route.method]({path: route.path, version: route.version}, chain);
    var msg = 'successfully loaded route: ' + route.method.toUpperCase() + ' ' + route.path;
    if (route.version) msg += ' [' + route.version + ']';
    log.info(msg);
  });
}
module.exports.loadRoutes = loadRoutes;