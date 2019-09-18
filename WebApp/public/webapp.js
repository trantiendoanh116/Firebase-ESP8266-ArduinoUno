
var homeModule = angular.module('myApp', ['ngRoute', 'mobile-angular-ui']);
homeModule.config(function ($routeProvider) {
	$routeProvider.when('/', {
		templateUrl: 'home.html',
		controller: 'Home'
	});
});
homeModule.factory('database', function () {
	//init firebase
	var firebaseConfig = {
		apiKey: "AIzaSyCZkXut96ZTsANIFBjxqsxI6eWFg1C5YXM",
		authDomain: "smarthome-116.firebaseapp.com",
		databaseURL: "https://smarthome-116.firebaseio.com",
		projectId: "smarthome-116",
		storageBucket: "smarthome-116.appspot.com",
		messagingSenderId: "1055864949441",
		appId: "1:1055864949441:web:ea1474b982167625cde240"
	};
	firebase.initializeApp(firebaseConfig);
	return firebase.database();
	
});
//Hàm xử lý chính
homeModule.controller('Home', function ($scope, database) {
	var pathValues = '/values';
	var pathAction = '/action';
	$scope.fan_status = 0;
	$scope.light_status = 0;
	$scope.apt_status = 0;
	$scope.co2_value = 0;
	$scope.temp_value = 0;
	$scope.humi_value = 0;
	database.ref(pathValues).on('value', function (snapshot) {

			console.log(snapshot.val());
			$scope.fan_status = snapshot.val().fan;
			$scope.light_status = snapshot.val().light;
			$scope.apt_status = snapshot.val().apt;
			$scope.co2_value = snapshot.val().co2;
			$scope.temp_value = snapshot.val().temp;
			$scope.humi_value = snapshot.val().humi;
			$scope.$apply();
	});

	$scope.onFanClick = function () {
		database.ref(pathAction+'/on_fan').set(true);
		console.log("On FAN");
	};


	$scope.offFanClick = function () {
		console.log("OFF FAN");
		database.ref(pathAction+'/off_fan').set(true);
	};



	$scope.changeLight = function () {
		console.log("ChangeLight");
		database.ref(pathAction+'/change_light').set(true);
	};


	$scope.changeApt = function () {
		console.log("ChangeAtomat");
		database.ref(pathAction+'/change_apt').set(true);
	};

});

