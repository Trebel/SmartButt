'use strict';

angular.module('testApp')
  .controller('MainCtrl', function ($scope) {
    $scope.awesomeThings = [
      'HTML5 Boilerplate',
      'AngularJS',
      'Karma'
    ];
  });

$http({
    url: "/api/toilets",
    method: "GET",
    headers: {
        "Content-Type": "application/json; charset=utf-8"
    }
}).success(function(response){
    $scope.loos = response;
}).error(function(error){
    $scope.error = error;
});