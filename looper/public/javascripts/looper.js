disableEventCreate = false;
showPalette = false;

//function ComputationInterface(options) {
function Looper(options) {
    var defaults = {
        devices: []
    };
    var options = options || {};
    var options = $.extend({}, defaults, options);

    this.devices = [];

    this.commands = [];
    this.commands.push("turn on");
    this.commands.push("turn off");
    this.commands.push("delay");

    function addDevice() {

        deviceCount = deviceCount + 1;
        $('#panes').append('<li class="pane' + deviceCount + '"><canvas id="canvas' + deviceCount + '" style="width: 100%; height: 100%;"></canvas></li>');
        canvas = "canvas" + deviceCount;

        var device = new Device({ canvas: canvas });
        device.looper = this;
        setupGestures(device);
        this.devices.push(device);

        /**
         * Re-initialize Carousel after adding the new device pane
         */
        this.carousel = new Carousel("#carousel");
        this.carousel.init();
        // this.carousel.showPane(deviceCount);
    }
    this.addDevice = addDevice;

    function showDeviceByIndex(index) {
        this.carousel.showPane(index + 1);
    }
    this.showDeviceByIndex = showDeviceByIndex;
}

function EventLoop(options) {
    var defaults = {
        events: []
    };
    var options = options || {};
    var options = $.extend({}, defaults, options);

    this.events = options.events; // events on the event loop
}

function Event(options) {
    var defaults = {
        x: null,
        y: null,
        xTarget: null,
        yTarget: null,
        state: 'INVALID', // INVALID, FLOATING, MOVING, ENTANGLED, SEQUENCED
        //visible: true
        go: null
    };
    var options = options || {};
    var options = $.extend({}, defaults, options);

    this.x = options.x;
    this.y = options.y;

    this.xTarget = options.xTarget;
    this.yTarget = options.yTarget;

    this.state = options.state;

    this.go = options.go;

    //this.visible = options.visible;
}





function setupGestures(device) {

    var currentCanvas = '#' + device.canvas;

    /**
     * Touch event handler
     * @param  {[type]} ev [description]
     * @return {[type]}    [description]
     */
    $(currentCanvas).hammer({ drag_max_touches: 0 }).on("touch", function(ev) {
        console.log("'touch' event!");

        var touches = ev.gesture.touches;

        //
        // Get the touched event node, if one exists
        // 
        var eventCount = device.processingInstance.eventLoop.events.length;

        for (var i = 0; i < eventCount; i++) {
            var loopEvent = device.processingInstance.eventLoop.events[i];
            if ((ev.gesture.center.pageX - 50 < loopEvent.x && loopEvent.x < ev.gesture.center.pageX + 50)
                && (ev.gesture.center.pageY - 50 < loopEvent.y && loopEvent.y < ev.gesture.center.pageY + 50)) {

                //loopEvent.visible = false;
                loopEvent.state = 'MOVING';
                disableEventCreate = true;

                var index = Math.random() * 2;
                loopEvent.go = device.looper.commands[parseInt(index)];

                console.log("\tevent " + i);
                break;
            }
        }

        //
        // Check of "go" button touched
        //
        
        if ((ev.gesture.center.pageX - 50 < (device.processingInstance.screenWidth / 2) && (device.processingInstance.screenWidth / 2) < ev.gesture.center.pageX + 50)
            && (ev.gesture.center.pageY - 50 < (device.processingInstance.screenHeight / 2) && (device.processingInstance.screenHeight / 2) < ev.gesture.center.pageY + 50)) {

            //console.log("go");
            device.processingInstance.getEventSequence();
        }

        //
        // Check if "script" button touched
        //
        
        if ((ev.gesture.center.pageX - 50 < (device.processingInstance.screenWidth / 2) && (device.processingInstance.screenWidth / 2) < ev.gesture.center.pageX + 50)
            && (ev.gesture.center.pageY - 50 < (device.processingInstance.screenHeight / 7 + 20) && (device.processingInstance.screenHeight / 7 + 20) < ev.gesture.center.pageY + 50)) {

            // Prompt for new script name
            var newScriptName = prompt("Type a name for this script", "script");
            if (newScriptName.length > 0) {
                scriptName = newScriptName;
            }
        }

        ev.gesture.preventDefault();
        ev.stopPropagation();
        ev.gesture.stopPropagation();
        return;
    });

    $(currentCanvas).hammer({ drag_max_touches: 0 }).on("release", function(ev) {
        console.log("'release' event!");

        var touches = ev.gesture.touches;

        // Get the touched object, if one exists
        var eventCount = device.processingInstance.eventLoop.events.length;
        for (var i = 0; i < eventCount; i++) {
            var loopEvent = device.processingInstance.eventLoop.events[i];

            if (loopEvent.state === 'MOVING') {

                // deltaX = ev.gesture.center.pageX - (screenWidth / 2);
                // deltaY = ev.gesture.center.pageY - (screenHeight / 2);
                // //angleInDegrees = Math.atan(deltaY / deltaX) * 180 / PI;
                // angleInDegrees = Math.atan2(deltaY, deltaX); // * 180 / PI;

                // x = screenWidth / 2 + (400 / 2) * Math.cos(angleInDegrees);
                // y = screenHeight / 2 + (400 / 2) * Math.sin(angleInDegrees);

                var distance = device.processingInstance.getDistanceFromEventLoop(loopEvent);
                console.log(distance);

                console.log('DISTANCE');
                console.log(distance);

                if (distance < 110) {
                    var nearestPosition = device.processingInstance.getNearestPositionOnEventLoop(ev.gesture.center.pageX, ev.gesture.center.pageY);
                    loopEvent.x = nearestPosition.x;
                    loopEvent.y = nearestPosition.y;
                    loopEvent.state = 'SEQUENCED';
                } else {
                    loopEvent.state = 'FLOATING';
                }

                // TODO: Only show JS editor by default for new nodes
                console.log("open editor!");
                // $('#overlay').show(); // TODO: Append argument

                disableEventCreate = false;

                break;
            }
        }

        disableEventCreate = false;

        // ev.gesture.preventDefault();
        // ev.stopPropagation();
        // ev.gesture.stopPropagation();
        // return;
    });



    $(currentCanvas).hammer({ drag_max_touches: 0, hold_timeout: 200 }).on("hold", function(ev) {
        console.log("'hold' event!");

        if (!disableEventCreate) {
            disableEventCreate = true;

            var touches = ev.gesture.touches;

            // deltaX = ev.gesture.center.pageX - (screenWidth / 2);
            // deltaY = ev.gesture.center.pageY - (screenHeight / 2);
            // //angleInDegrees = Math.atan(deltaY / deltaX) * 180 / PI;
            // angleInDegrees = Math.atan2(deltaY, deltaX); // * 180 / PI;

            // x = screenWidth / 2 + (400 / 2) * Math.cos(angleInDegrees);
            // y = screenHeight / 2 + (400 / 2) * Math.sin(angleInDegrees);

            var nearestPosition = device.processingInstance.getNearestPositionOnEventLoop(ev.gesture.center.pageX, ev.gesture.center.pageY);

            // xPalette = device.processingInstance.mouseX;
            // yPalette = device.processingInstance.mouseY;
            // showPalette = true;

            var loopEvent = new Event({
                x: ev.gesture.center.pageX,
                y: ev.gesture.center.pageY,
                xTarget: nearestPosition.x,
                yTarget: nearestPosition.y
            });

            loopEvent.state = 'MOVING';
            device.processingInstance.eventLoop.events.push(loopEvent);
        }

        ev.gesture.preventDefault();
        ev.stopPropagation();
        ev.gesture.stopPropagation();
        return;
    });
}

// devices = [];
deviceCount = -1;

/**
 * Add an expressive interface to a device.
 */
function Device(options) {

    /** Set up object defaults */
    var defaults = {
        canvas: null, // Canvas instance
        // eventLoop: null
    };
    var options = options || {};
    var options = $.extend({}, defaults, options);

    if (options.canvas === null) {
        alert("No canvas specified.");
        return;
    }

    this.canvas = options.canvas;

    this.scriptName = "script";
    // this.disableEventCreate = false;
    this.showPalette = false;
    this.font = null;

    /**
     * Processing sketch code
     */
    var deviceSketch = new Processing.Sketch(function(processing) {

        var backgroundColor = processing.color(Math.random() * 255, Math.random() * 255, Math.random() * 255);
        function generateRandomColor(red, green, blue) {
            // Random random = new Random();
            var randomRed = Math.random() * 255;
            var randomGreen = Math.random() * 255;
            var randomBlue = Math.random() * 255;

            // mix the color
            if (red !== null && green !== null && blue !== null) {
                randomRed = (randomRed + red) / 2;
                randomGreen = (randomGreen + green) / 2;
                randomBlue = (randomBlue + blue) / 2;
            }

            // Color color = new Color(red, green, blue);
            var color = { red: randomRed, green: randomGreen, blue: randomBlue };
            return color;
        }
        var color = generateRandomColor(255, 255, 255);
        var backgroundColor = processing.color(color.red, color.green, color.blue);

        var scriptName = "script";

        processing.eventLoop = new EventLoop();

        // Override setup function
        processing.setup = function() {
            processing.size(processing.screenWidth, processing.screenHeight);

            this.font = processing.loadFont("DidactGothic.ttf");
        }

        // Override draw function, by default it will be called 60 times per second
        processing.draw = function() {

            function drawEventLoop() {

                processing.pushMatrix();

                // Draw the "loop" circular arc
                processing.strokeWeight(1.0);
                processing.stroke(65, 65, 65);
                processing.noFill();
                processing.smooth();
                processing.arc(processing.screenWidth / 2, processing.screenHeight / 2, 400, 400, (-processing.PI/2) + 0.05*processing.PI, 1.45*processing.PI);

                // Draw arrow
                processing.translate(processing.screenWidth / 2, processing.screenHeight / 2);
                processing.translate(-29, -198);
                processing.rotate(-0.05*processing.PI);
                processing.line(0, 0, -16, 16);
                processing.line(0, 0, -16, -16);

                processing.popMatrix();
            }

            function drawEvents() {

                processing.pushMatrix();

                var eventCount = processing.eventLoop.events.length;
                for (var i = 0; i < eventCount; i++) {
                    var loopEvent = processing.eventLoop.events[i];

                    processing.updatePosition(loopEvent);

                    // Draw the event node
                    processing.fill(66, 214, 146);
                    processing.ellipse(loopEvent.x, loopEvent.y, 60, 60);

                    // Calculate nearest point on circle
                    //line(loopEvent.x, loopEvent.y, screenWidth / 2, screenHeight / 2);
                }

                processing.popMatrix();
            }

            /**
             * Updates position of event node.
             * @param  {[type]} loopEvent [description]
             * @return {[type]}           [description]
             */
            function updatePosition(loopEvent) {

                if (loopEvent.x !== loopEvent.xTarget || loopEvent.y !== loopEvent.yTarget) { // FREE
                    //console.log("Rendering ghost");
                }
                
                // Check if event node is near to the event loop
                // if (distance < 110) { // ENTANGLED
                //     loopEvent.state = 'ENTANGLED';
                // } else {
                //     loopEvent.state = 'FLOATING';
                // }

                if (loopEvent.state === 'MOVING') {

                    // console.log(processing.mouseX);
                    // console.log($('#' + processing.canvas).offset());
                    console.log(processing.screenWidth * (processing.deviceCount + 1) + processing.mouseX);
                    console.log(processing.mouseY);
                    // console.log($(processing.canvas).offset());

                    // Standard update for a moving event
                    currentMouseX = processing.screenWidth * (processing.deviceCount + 1) + processing.mouseX;
                    loopEvent.x = currentMouseX;
                    loopEvent.y = processing.mouseY;

                    deltaX = currentMouseX - (processing.screenWidth / 2);
                    deltaY = processing.mouseY - (processing.screenHeight / 2);
                    angleInDegrees = Math.atan2(deltaY, deltaX);

                    loopEvent.xTarget = processing.screenWidth / 2 + (400 / 2) * Math.cos(angleInDegrees);
                    loopEvent.yTarget = processing.screenHeight / 2 + (400 / 2) * Math.sin(angleInDegrees);

                    // Check if under certain distance from the circle (and attach to)
                    var distance = processing.lineDistance(loopEvent.x, loopEvent.y, loopEvent.xTarget, loopEvent.yTarget);

                    if (distance < 110) { // ENTANGLED
                    //if (loopEvent.state === 'ENTANGLED') {
                        processing.line(loopEvent.x, loopEvent.y, loopEvent.xTarget, loopEvent.yTarget);

                        // Draw the "would be" position that the event node would occupy
                        processing.fill(66, 214, 146, 50);
                        processing.ellipse(loopEvent.xTarget, loopEvent.yTarget, 50, 50);

                        // Snap to event loop
                        if (!disableEventCreate) {
                            deltaX = processing.mouseX - (processing.screenWidth / 2);
                            deltaY = processing.mouseY - (processing.screenHeight / 2);
                            //angleInDegrees = Math.atan(deltaY / deltaX) * 180 / PI;
                            angleInDegrees = Math.atan2(deltaY, deltaX); // * 180 / PI;

                            loopEvent.x = processing.screenWidth / 2 + (400 / 2) * Math.cos(angleInDegrees);
                            loopEvent.y = processing.screenHeight / 2 + (400 / 2) * Math.sin(angleInDegrees);
                        }
                    }

                } else if (loopEvent.state === 'FLOATING') {
                    processing.fill(66, 214, 146, 50);
                    processing.ellipse(loopEvent.x, loopEvent.y, 70, 70);
                }
            }
            processing.updatePosition = updatePosition;

            /**
             * Returns the sequence of events in the event queue.
             * @return {[type]} [description]
             */
            function getEventSequence() {
                var eventSequence = [];

                var eventCount = processing.eventLoop.events.length;

                // Populate array for sorting
                for (var i = 0; i < eventCount; i++) {
                    var loopEvent = processing.eventLoop.events[i];
                    if (loopEvent.state === 'SEQUENCED') {
                        // loopEvent.go = function() { console.log("action " + i); }
                        loopEvent.go = function() { 
                            // var command = loopEvent.go;
                            var index = Math.random() * 2;
                            // command = this.looper.commands[parseInt(index)];
                            command = "turn on";
                            console.log("instr: " + command);
                        }
                        eventSequence.push({
                            event: loopEvent,
                            angle: getAngle(loopEvent.x, loopEvent.y)
                        });
                    }
                }

                // Perform insertion sort
                var i, j;
                var loopEvent;
                eventCount = eventSequence.length;
                for (var i = 0; i < eventCount; i++) {
                    loopEvent = eventSequence[i];

                    for (j = i-1; j > -1 && eventSequence[j].angle > loopEvent.angle; j--) {
                        eventSequence[j+1] = eventSequence[j];
                    }

                    eventSequence[j+1] = loopEvent;
                }

                console.log(eventSequence);

                for (var i = 0; i < eventSequence.length; i++) {
                    loopEvent = eventSequence[i];

                    loopEvent.event.go();
                }

                return eventSequence;
            }
            processing.getEventSequence = getEventSequence;

            function lineDistance(x1, y1, x2, y2) {
                var xs = 0;
                var ys = 0;

                xs = x2 - x1;
                xs = xs * xs;

                ys = y2 - y1;
                ys = ys * ys;

                return Math.sqrt(xs + ys);
            }
            processing.lineDistance = lineDistance;

            function getPointOnCircle(radius, originX, originY, angle) {
                //var radius = 50;
                //var originX = 400;
                //var originY = 400;
                //var angle = 40;

                x = originX + radius * Math.cos(angle);
                y = originY + radius * Math.sin(angle);

                var result = { x: x, y: y };

                return result;
            }

            function getPosition(angle) {

                var nearestX = processing.screenWidth / 2 + (400 / 2) * Math.cos(angle);
                var nearestY = processing.screenHeight / 2 + (400 / 2) * Math.sin(angle);

                var nearestPosition = {
                    x: nearestX,
                    y: nearestY
                };
                return nearestPosition;
            }
            processing.getPosition = getPosition;

            function getAngle(x, y) {
                var deltaX = x - (processing.screenWidth / 2);
                var deltaY = y - (processing.screenHeight / 2);
                var angleInRadians = Math.atan2(deltaY, deltaX); // * 180 / PI;
                if (angleInRadians < 0) {
                    angleInRadians = Math.PI + (Math.PI + angleInRadians);
                }
                angleInRadians = angleInRadians + (Math.PI / 2); // Offset by (PI / 2) radians
                if (angleInRadians > (2 * Math.PI)) {
                    angleInRadians = angleInRadians - (2 * Math.PI);
                }
                return angleInRadians;
            }

            function getNearestPositionOnEventLoop(x, y) {
                var deltaX = x - (processing.screenWidth / 2);
                var deltaY = y - (processing.screenHeight / 2);
                var angleInDegrees = Math.atan2(deltaY, deltaX); // * 180 / PI;

                var nearestX = processing.screenWidth / 2 + (400 / 2) * Math.cos(angleInDegrees);
                var nearestY = processing.screenHeight / 2 + (400 / 2) * Math.sin(angleInDegrees);

                var nearestPosition = {
                    x: nearestX,
                    y: nearestY
                };
                return nearestPosition;
            }
            processing.getNearestPositionOnEventLoop = getNearestPositionOnEventLoop;

            function getDistanceFromEventLoop(loopEvent) {
                var distance = processing.lineDistance(loopEvent.x, loopEvent.y, loopEvent.xTarget, loopEvent.yTarget);
                return distance;
            }
            processing.getDistanceFromEventLoop = getDistanceFromEventLoop;

            /** Start of actual loop instructions. The things above are definitions. */

            // erase background
            processing.background(backgroundColor);

            // draw "go" button
            primaryFont = processing.createFont("DidactGothic.ttf", 32);
            processing.textFont(primaryFont, 100);
            processing.textAlign(processing.CENTER);
            processing.fill(65, 65, 65);
            processing.text("go", processing.screenWidth / 2, processing.screenHeight / 2 + 20);

            // draw script name
            primaryFont = processing.createFont("DidactGothic.ttf", 16);
            processing.textFont(primaryFont, 40);
            processing.textAlign(processing.CENTER);
            processing.fill(65, 65, 65);
            processing.text(scriptName, processing.screenWidth / 2, processing.screenHeight / 7 + 20);

            drawEventLoop();
            drawEvents();
        };
    });

    // deviceSketch.options.crispLines = true;
    this.processingInstance = new Processing(canvas, deviceSketch);
    this.processingInstance.canvas = canvas;
    this.processingInstance.deviceCount = deviceCount;
    // this.eventLoop = this.processingInstance.eventLoop;
    console.log(this.processingInstance);
}