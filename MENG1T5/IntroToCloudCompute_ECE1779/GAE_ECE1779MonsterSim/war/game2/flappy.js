//copied from:
//http://www.lostdecadegames.com/how-to-make-a-simple-html5-canvas-game/
//modified to make flappy birds


//NON GAME CODE

function myFunction(String)
{
	document.getElementById("comment").innerHTML="Submitting the below scores:";
	document.getElementById("tot_score").innerHTML="Total Score: " + total_score;
	document.getElementById("high_score").innerHTML="High Score: " +high_score;
	
	//document.getElementById("feed_button").disabled = true;
	//document.getElementById("return_button").disabled = true;
	
	hero.speed=0;
	var form = document.createElement("form");
	var high_score_f = document.createElement("input");
	var tot_score_f=document.createElement("input");
	
	form.name = "input";
	form.method = "post";
	form.action = "game_stats.jsp";

    
	high_score_f.type="number";
	high_score_f.name="high_score";
	high_score_f.value=high_score;
	
	
	tot_score_f.type="number";
	tot_score_f.name="tot_score";
	tot_score_f.value=total_score;
	

	
    form.appendChild(high_score_f);
    form.appendChild(tot_score_f);


    var delay=3000//3 seconds
	setTimeout(function(){
		form.submit();
	},delay)
	
    
};




// Create the canvas

var canvas = document.getElementById("gameCanvas");
var ctx = canvas.getContext("2d");



//load avatar
var avatar_url = document.getElementById("helper").getAttribute("data-name");

// Background image
var bgReady = false;
var bgImage = new Image();
bgImage.onload = function () {
	bgReady = true;
};
bgImage.src = "game2/white.png";

// Hero image
var heroReady = false;
var heroImage = new Image();
heroImage.onload = function () {
	heroReady = true;
};

var avatar_loc="icons/old_icons/";
var avatar_type=".bmp"
var avatar_string = avatar_loc.concat(avatar_url).concat(avatar_type) 

heroImage.src = avatar_string;

// Monster image
var monsterReady = false;
var monsterImage = new Image();
monsterImage.onload = function () {
	monsterReady = true;
};
monsterImage.src = "icons/old_icons/earth.bmp";

// Game objects
var pipe = {
		length: Math.random() * (canvas.height/2 - 80)
};

//new pipe variables
//var screen_height = canvas.height;
var GAP_HEIGHT= 150;
var HERO_OFFSET_FACTOR = 5;

//this will be the random function determining the gap position for added challenge
var gap_position= Math.random() * (0.7*canvas.height-0.3*canvas.height) + 0.3*canvas.height;
//var gap_position= 20;
var PIPE_WIDTH=60;


var pipe_pos = canvas.width;
var passed_pipe=false;
var pipe_score=0;
var high_score=0;
var total_score=0;

var hero = {
	speed: 256 // movement in pixels per second
};
var monster = {};
var monstersCaught = 0;
var paused=false;

var PIPE_SPEED_MOD=2.3;
var GRAVITY_MOD=0.9;
var JUMP_MOD = 1.9;


var HERO_H=70;
var HERO_W=76;


//Handle keyboard controls
var keysDown = {};
//var keysPress={};

addEventListener("keydown", function (e) {
	keysDown[e.keyCode] = true;
}, false);


addEventListener("keyup", function (e) {
	delete keysDown[e.keyCode];
}, false);

// Reset the game when the player catches a monster
var reset = function () {
	
	
	hero.x = canvas.width /HERO_OFFSET_FACTOR;
	hero.y = canvas.height / 2;
	
	pipe_pos = canvas.width;
	//pipe.length =  Math.random() * (canvas.height/2 - 80);
	
	if (pipe_score >high_score){
		high_score = pipe_score;
	}
	//total_score += pipe_score;
	pipe_score=0;
	
	gap_position= Math.random() * (0.7*canvas.height-0.3*canvas.height) + 0.3*canvas.height;
	
	
	// Throw the monster somewhere on the screen randomly
	//monster.x = 32 + (Math.random() * (canvas.width - 64));
	//monster.y = 32 + (Math.random() * (canvas.height - 64));
};



// Update game objects
var update = function (modifier) {
	
	
	pipe_pos -= hero.speed * modifier * PIPE_SPEED_MOD;
	

	
	//check if collision w/ pipe
	
	hero.y += hero.speed*modifier*GRAVITY_MOD;
	
	if (38 in keysDown) { // Player holding up
		hero.y -= hero.speed * modifier * JUMP_MOD;
		//pipe_pos -= hero.speed * modifier;
		
	}
	

	

	
	//has pipe reached end?
	if (pipe_pos + PIPE_WIDTH <= 0) {
		pipe_pos = canvas.width;
		//pipe.length =  Math.random() * (canvas.height/2 - 80);
		gap_position= Math.random() * (0.7*canvas.height-0.3*canvas.height) + 0.3*canvas.height;
		passed_pipe=false;
	}
	
	
	//is hero behind or in front of pipe?
	if (hero.x > pipe_pos+PIPE_WIDTH && !passed_pipe) {
		
		passed_pipe=true;
		++pipe_score;
		++total_score;
	}
	


	
	//check collision w/ pipe
	if (hero.x + (HERO_W) >= pipe_pos && hero.x <= pipe_pos+PIPE_WIDTH  && !passed_pipe){
		if (hero.y + HERO_H < gap_position+GAP_HEIGHT && hero.y > gap_position ) {
			//++monstersCaught;
			
			//passed_pipe=false;

		}
		//collision occured
		else {
			reset();
			
			
		}
	}
	
};

// Draw everything
var render = function () {
	if (bgReady) {
		ctx.drawImage(bgImage, 0, 0);
	}

	if (heroReady) {
		ctx.drawImage(heroImage, hero.x, hero.y);
	}

	if (monsterReady) {
		ctx.drawImage(monsterImage, monster.x, monster.y);
	}

	
    
    //draw new set of pipes
    //upper pipe
    ctx.beginPath();
    ctx.rect(pipe_pos, 0, PIPE_WIDTH, gap_position);
    ctx.fillStyle = 'green';
    ctx.fill();
    ctx.lineWidth = 3;
    ctx.strokeStyle = 'black';
    ctx.stroke();
    
    //lower pipe
    ctx.beginPath();
    ctx.rect(pipe_pos, gap_position+GAP_HEIGHT, PIPE_WIDTH, canvas.height-gap_position-GAP_HEIGHT);
    ctx.fillStyle = 'green';
    ctx.fill();
    ctx.lineWidth = 3;
    ctx.strokeStyle = 'black';
    ctx.stroke();

    
    
	// Score
	
	
	ctx.fillStyle = "rgb(25, 25, 25)";
	ctx.font = "24px Helvetica";
	ctx.textAlign = "left";
	ctx.textBaseline = "top";
	ctx.fillText("Score: " + pipe_score, 32, 32);
	
	
	//total score
	ctx.fillStyle = "rgb(25, 25, 25)";
	ctx.font = "24px Helvetica";
	ctx.textAlign = "left";
	ctx.textBaseline = "top";
	ctx.fillText("High Score: " + high_score, 32, 62);
	
	//high score
	ctx.fillStyle = "rgb(25, 25, 25)";
	ctx.font = "24px Helvetica";
	ctx.textAlign = "left";
	ctx.textBaseline = "top";
	ctx.fillText("Total Score: " + total_score, 32, 92);
};


// The main game loop
var main = function () {
	var now = Date.now();
	var delta = now - then;

	update(delta / 1000);
	render();

	then = now;
};

// Let's play this game!
reset();
alert("Please press OK to start!");

var then = Date.now();
setInterval(main, 1); // Execute as fast as possible