disableEventCreate = false;
showPalette = false;




/**
    * super simple carousel
    * animation between panes happens with css transitions
    */
    function Carousel(element) {
        var self = this;
        element = $(element);

        var container = $(">ul", element);
        var panes = $(">ul>li", element);

        var pane_width = 0;
        var pane_count = panes.length;

        var current_pane = 0;


        /**
         * initial
         */
        this.init = function() {
            setPaneDimensions();

            $(window).on("load resize orientationchange", function() {
                setPaneDimensions();
                //updateOffset();
            })
        };


        /**
         * set the pane dimensions and scale the container
         */
        function setPaneDimensions() {
            pane_width = element.width();
            panes.each(function() {
                $(this).width(pane_width);
            });
            container.width(pane_width*pane_count);
        };


        /**
         * show pane by index
         * @param   {Number}    index
         */
        this.showPane = function( index ) {
            // between the bounds
            index = Math.max(0, Math.min(index, pane_count-1));
            current_pane = index;

            var offset = -((100/pane_count)*current_pane);
            setContainerOffset(offset, true);
        };


        function setContainerOffset(percent, animate) {
            container.removeClass("animate");

            if(animate) {
                container.addClass("animate");
            }

            if(Modernizr.csstransforms3d) {
                container.css("transform", "translate3d("+ percent +"%,0,0) scale3d(1,1,1)");
            }
            else if(Modernizr.csstransforms) {
                container.css("transform", "translate("+ percent +"%,0)");
            }
            else {
                var px = ((pane_width*pane_count) / 100) * percent;
                container.css("left", px+"px");
            }
        }

        this.next = function() { return this.showPane(current_pane+1, true); };
        this.prev = function() { return this.showPane(current_pane-1, true); };



        function handleHammer(ev) {
            // console.log(ev);
            // disable browser scrolling
            ev.gesture.preventDefault();

            if (!disableEventCreate) {

                switch(ev.type) {
                    case 'dragright':
                    case 'dragleft':
                        // stick to the finger
                        var pane_offset = -(100/pane_count)*current_pane;
                        var drag_offset = ((100/pane_width)*ev.gesture.deltaX) / pane_count;

                        // slow down at the first and last pane
                        if((current_pane == 0 && ev.gesture.direction == Hammer.DIRECTION_RIGHT) ||
                            (current_pane == pane_count-1 && ev.gesture.direction == Hammer.DIRECTION_LEFT)) {
                            drag_offset *= .4;
                        }

                        setContainerOffset(drag_offset + pane_offset);
                        break;

                    case 'swipeleft':
                        self.next();
                        ev.gesture.stopDetect();
                        break;

                    case 'swiperight':
                        self.prev();
                        ev.gesture.stopDetect();
                        break;

                    case 'release':
                        // more then 50% moved, navigate
                        if(Math.abs(ev.gesture.deltaX) > pane_width/2) {
                            if(ev.gesture.direction == 'right') {
                                self.prev();
                            } else {
                                self.next();
                            }
                        }
                        else {
                            self.showPane(current_pane, true);
                        }
                        break;
                }

            }
        }

        element.hammer({ drag_lock_to_axis: true })
            .on("release dragleft dragright swipeleft swiperight", handleHammer);
    }


    var carousel = new Carousel("#carousel");
    carousel.init();

    // $(".drag")
    //   .hammer({ drag_max_touches: 0 })
    //   .on("touch drag dragright dragleft", function(ev) {
    //     console.log("DRAG CIRCLE");

    //     var touches = ev.gesture.touches;

    //     for(var t=0,len=touches.length; t<len; t++) {
    //         var target = $(touches[t].target);
    //         target.css({
    //             zIndex: 1337,
    //             left: touches[t].pageX-50,
    //             top: touches[t].pageY-50
    //         });
    //     }

    //     ev.gesture.preventDefault();
    //     ev.stopPropagation();
    //     ev.gesture.stopPropagation();
    //     return;
    //   });

//------------
// looper.js
//------------

// TODO: Define a domain-specific language for the microcontroller.
function saveScript() {
    var script = "" + firepadDevice.firepad.getText();
    firepadEvent.behavior = eval('(' + script + ')');
}

/**
 * The main Looper class.
 */
function Looper(options) {
    var defaults = {
        devices: [],
        going: false
    };
    var options = options || {};
    var options = $.extend({}, defaults, options);

    /** The behavior palette. */
    this.palette = null;

    /** The devices in the local mesh network. */
    this.devices = [];

    /** The domain-specific language. */
    this.commands = [];
    this.commands.push("turn on");
    this.commands.push("turn off");
    this.commands.push("delay");

    /**
     * Add a device to the list of devices in the mesh network.
     */
    function addDevice() {

        deviceCount = deviceCount + 1;

        var overlay = '';
        overlay += '<div id="overlay' + deviceCount + '" style="width: 100%; height: 100%; position: relative; z-index: 5000;">';
        overlay += '<input type="button" value="close" onclick="saveScript();$(\'#overlay' + deviceCount + '\').hide();" />';
        overlay += '</div>';
        // <script>
        //     $('#overlay').hide();
        // </script>


        $('#panes').append('<li class="pane' + deviceCount + '">' + overlay + '<canvas id="canvas' + deviceCount + '" style="width: 100%; height: 100%;"></canvas></li>');
        canvas = "canvas" + deviceCount;

        var device = new Device({ canvas: canvas });
        device.looper = this;
        device.index = deviceCount; // TODO: Replace with node/node UUID
        setupGestures(device);
        this.devices.push(device);

        /**
         * Re-initialize Carousel after adding the new device pane
         */
        this.carousel = new Carousel("#carousel");
        this.carousel.init();
        // this.carousel.showPane(deviceCount);

        $('#overlay' + deviceCount).hide();
    }
    this.addDevice = addDevice;

    function showDeviceByIndex(index) {
        this.carousel.showPane(index + 1);
    }
    this.showDeviceByIndex = showDeviceByIndex;
}

function EventLoop(options) {
    var defaults = {
        events: [],
        going: false,
        position: 0
    };
    var options = options || {};
    var options = $.extend({}, defaults, options);

    this.events = options.events; // events on the event loop

    this.position = options.position;
    this.going = options.going;

    function go() {
        this.updateLoopOrdering();
        this.going = true;
    }
    this.go = go;

    function stop() {
        this.going = false;
        this.updateLoopOrdering();

        // Stop all events in the event loop
        for (var i = 0; i < this.events.length; i++) {
            this.events[i].stop();
        }
        this.position = 0; // Reset position
    }
    this.stop = stop;

    function toggle() {
        if (this.going) {
            this.stop();
        } else {
            this.go();
        }
    }
    this.toggle = toggle;

    function step() {
        if (this.going) {
            var previousEvent = this.events[this.position];
            previousEvent.stop();

            this.position = (this.position + 1) % this.events.length;
            // console.log('new position = ' + this.position);

            var currentEvent = this.events[this.position];
            currentEvent.go();

            currentEvent.behavior();


            // this.getEventSequence();
            // for (var i = 0; i < eventSequence.length; i++) {
            //     loopEvent = eventSequence[i];

            //     loopEvent.event.go();
            // }
        }
    }
    this.step = step;

    function getAngle(x, y) {
        var deltaX = x - ($(window).width() / 2);
        var deltaY = y - ($(window).height() / 2);
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
    // processing.getAngle;

    /**
     * Re-orders the events in the event loop.
     */
    function updateLoopOrdering() {
        var eventSequence = [];

        var eventCount = this.events.length;

        // Populate array for sorting
        for (var i = 0; i < eventCount; i++) {
            var loopEvent = this.events[i];
            if (loopEvent.state === 'SEQUENCED') {
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

        // console.log(eventSequence);

        // for (var i = 0; i < eventSequence.length; i++) {
        //     loopEvent = eventSequence[i];

        //     // console.log(loopEvent);

        //     var behaviorScript = loopEvent.event.behavior;
        // }

        //return eventSequence;

        var updatedEventLoop = [];
        for (var i = 0; i < eventSequence.length; i++) {
            loopEvent = eventSequence[i];
            updatedEventLoop.push(loopEvent.event);
        }

        this.events = updatedEventLoop;
    }
    this.updateLoopOrdering = updateLoopOrdering;
}

function Event(options) {
    var defaults = {
        x: null,
        y: null,
        xTarget: null,
        yTarget: null,
        state: 'INVALID', // INVALID, FLOATING, MOVING, ENTANGLED, SEQUENCED
        //visible: true
        behavior: null,
        going: false,
        label: '?'
    };
    var options = options || {};
    var options = $.extend({}, defaults, options);

    this.x = options.x;
    this.y = options.y;

    this.xTarget = options.xTarget;
    this.yTarget = options.yTarget;

    this.state = options.state;

    this.go = options.go;

    this.label = options.label;

    //this.visible = options.visible;
    this.going = options.going;

    function go() {
        this.going = true;
    }
    this.go = go;

    function stop() {
        this.going = false;
    }
    this.stop = stop;

    function toggle() {
        if (this.going) {
            this.going = false;
        } else {
            this.going = true;
        }
    }
    this.toggle = toggle;
}

function Behavior(options) {
    var defaults = {
        x: null,
        y: null,
        xTarget: null,
        yTarget: null,
        // state: 'INVALID', // INVALID, FLOATING, MOVING, ENTANGLED, SEQUENCED
        //visible: true
        // go: null,
        // going: false,
        label: '?',
        visible: false,
        script: null // The script to do the behavior. The "script" to run to execute the behavior.
    };
    var options = options || {};
    var options = $.extend({}, defaults, options);

    this.x = options.x;
    this.y = options.y;

    this.xTarget = options.xTarget;
    this.yTarget = options.yTarget;

    this.label = options.label;

    this.visible = options.visible;

    this.script = options.script;

    function setPosition(x, y) {
        this.xTarget = x;
        this.yTarget = y;
    }
    this.setPosition = setPosition;

    function updatePosition() {
        this.x = this.xTarget;
        this.y = this.yTarget;
    }
    this.updatePosition = updatePosition;

    function show() {
        this.visible = true;
    }
    this.show = show;

    function hide() {
        this.visible = false;
    }
    this.hide = hide;

    function toggle() {
        if (this.visible) {
            this.visible = false;
        } else {
            this.visible = true;
        }
    }
    this.toggle = toggle;
}

function BehaviorPalette(options) {
    var defaults = {
        x: null,
        y: null,
        xTarget: null,
        yTarget: null,
        // state: 'INVALID', // INVALID, FLOATING, MOVING, ENTANGLED, SEQUENCED
        //visible: true
        // go: null,
        // going: false,
        behaviors: [],
        label: '?',
        visible: false
    };
    var options = options || {};
    var options = $.extend({}, defaults, options);

    this.x = options.x;
    this.y = options.y;

    this.xTarget = options.xTarget;
    this.yTarget = options.yTarget;

    this.behaviors = options.behaviors;

    // this.state = options.state;

    this.label = options.label;

    this.visible = options.visible;

    function setPosition(x, y) {
        this.xTarget = x;
        this.yTarget = y;
    }
    this.setPosition = setPosition;

    function updatePosition() {
        this.x = this.xTarget;
        this.y = this.yTarget;
    }
    this.updatePosition = updatePosition;

    /**
     * Shows the behavior palette
     */
    function show() {
        this.visible = true;
    }
    this.show = show;

    /**
     * Hides the behavior palette
     */
    function hide() {
        this.visible = false;
    }
    this.hide = hide;

    /**
     * Toggles the visibility of the behavior palette
     */
    function toggle() {
        if (this.visible) {
            this.visible = false;
        } else {
            this.visible = true;
        }
    }
    this.toggle = toggle;

    /**
     * Adds a behavior node to the behavior palette
     */
    function addBehavior(x, y, label, script) {
        var behavior = new Behavior({
            x: x, // was ev.gesture.center.pageX,
            y: y, // was ev.gesture.center.pageY,
            xTarget: x,
            yTarget: y,
            label: label,

            // e.g.,
            // script: function() {
            //     console.log("DOING " + this.label);
            // }
            script: script
        });
        this.behaviors.push(behavior);
    }
    this.addBehavior = addBehavior;
}

function setupGestures(device) {

    var currentCanvas = '#' + device.canvas;

    /**
     * Handle "tap" events.
     */
    $(currentCanvas).hammer({ drag_max_touches: 0 }).on("tap", function(ev) {
        console.log("'tap' event!");

        var touches = ev.gesture.touches;

        //
        // Get the touched event node, if one exists
        //
        var eventCount = device.processingInstance.eventLoop.events.length;

        for (var i = 0; i < eventCount; i++) {
            var loopEvent = device.processingInstance.eventLoop.events[i];
            if ((ev.gesture.center.pageX - 50 < loopEvent.x && loopEvent.x < ev.gesture.center.pageX + 50)
                && (ev.gesture.center.pageY - 50 < loopEvent.y && loopEvent.y < ev.gesture.center.pageY + 50)) {

                // TODO: Handle "tap" event.
            }
        }
    });

    /**
     * Handle "touch" events.
     */
    $(currentCanvas).hammer({ drag_max_touches: 0 }).on("touch", function(ev) {
        console.log("'touch' event!");

        var touches = ev.gesture.touches;

        //
        // Check if palette option was selected
        //

        var behaviorPalette = device.processingInstance.behaviorPalette;

        if (behaviorPalette.visible) {

            var behaviorCount = device.processingInstance.behaviorPalette.behaviors.length;
            for(var i = 0; i < behaviorCount; i++) {
                var behavior = device.processingInstance.behaviorPalette.behaviors[i];

                // Check if palette option is touched
                if ((ev.gesture.center.pageX - 50 < behaviorPalette.x + behavior.x && behaviorPalette.x + behavior.x < ev.gesture.center.pageX + 50)
                    && (ev.gesture.center.pageY - 50 < behaviorPalette.y + behavior.y && behaviorPalette.y + behavior.y < ev.gesture.center.pageY + 50)) {

                    // Hide the behavior palette
                    device.processingInstance.behaviorPalette.visible = false;

                    // Create behavior node
                    var nearestPosition = device.processingInstance.getNearestPositionOnEventLoop(ev.gesture.center.pageX, ev.gesture.center.pageY);

                    var loopEvent = new Event({
                        x: device.processingInstance.behaviorPalette.x + behavior.x,
                        y: device.processingInstance.behaviorPalette.y + behavior.y,
                        xTarget: nearestPosition.x,
                        yTarget: nearestPosition.y
                    });

                    // Update for selected behavior
                    loopEvent.label = behavior.label;
                    loopEvent.behavior = behavior.script;

                    console.log(loopEvent);

                    // Update the state of the event node
                    loopEvent.state = 'MOVING';
                    device.processingInstance.eventLoop.events.push(loopEvent);
                }

            }
        }






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

                // var index = Math.random() * 2;
                // loopEvent.behavior = device.looper.commands[parseInt(index)];

                console.log("\tevent " + i);
                break;
            }
        }

        //
        // Check of "go" button touched
        //

        if ((ev.gesture.center.pageX - 50 < (device.processingInstance.screenWidth / 2) && (device.processingInstance.screenWidth / 2) < ev.gesture.center.pageX + 50)
            && (ev.gesture.center.pageY - 50 < (device.processingInstance.screenHeight - 100) && (device.processingInstance.screenHeight - 100) < ev.gesture.center.pageY + 50)) {

            var sequence = device.processingInstance.getEventSequence();

            // Start the event loop if any events exist
            if (sequence.length > 0) {
                //console.log("go");
                device.processingInstance.eventLoop.toggle(); // toggle "go" and "stop"
            }
        }

        //
        // Check if "script" button touched
        //

        if ((ev.gesture.center.pageX - 50 < (device.processingInstance.screenWidth / 2) && (device.processingInstance.screenWidth / 2) < ev.gesture.center.pageX + 50)
            && (ev.gesture.center.pageY - 50 < (device.processingInstance.screenHeight / 2 - 90) && (device.processingInstance.screenHeight / 2 - 90) < ev.gesture.center.pageY + 50)) {
            //&& (ev.gesture.center.pageY - 50 < (device.processingInstance.screenHeight / 7 + 20) && (device.processingInstance.screenHeight / 7 + 20) < ev.gesture.center.pageY + 50)) {

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

    /**
     * Detect "release" event.
     */
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

                if (distance < 110) {

                    // Update position of the event node and set as "sequenced"
                    var nearestPosition = device.processingInstance.getNearestPositionOnEventLoop(ev.gesture.center.pageX, ev.gesture.center.pageY);
                    loopEvent.x = nearestPosition.x;
                    loopEvent.y = nearestPosition.y;
                    loopEvent.state = 'SEQUENCED';

                    // Start the event loop if any events exist
                    var sequence = device.processingInstance.getEventSequence();
                    if (sequence.length > 0) {
                        //console.log("go");
                        device.processingInstance.eventLoop.go(); // toggle "go" and "stop"
                    }
                } else {

                    // Update position of the event node and set as "floating"
                    loopEvent.state = 'FLOATING';

                    // Stop the event loop if no nodes are placed on it
                    var sequence = device.processingInstance.getEventSequence();
                    if (sequence.length == 0) {
                        device.processingInstance.eventLoop.stop();
                    }
                }

                // Deploy code to Pixel module (i.e., the Espruino)
                device.deploy();

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

    /**
     * Handle "hold" touch event.
     */
    $(currentCanvas).hammer({ drag_max_touches: 0, hold_timeout: 200 }).on("hold", function(ev) {
        console.log("'hold' event!");

        //
        // Get the touched event node, if one exists
        //
        // var eventCount = device.processingInstance.eventLoop.events.length;

        // for (var i = 0; i < eventCount; i++) {
        //     var loopEvent = device.processingInstance.eventLoop.events[i];
        //     if ((ev.gesture.center.pageX - 50 < loopEvent.x && loopEvent.x < ev.gesture.center.pageX + 50)
        //         && (ev.gesture.center.pageY - 50 < loopEvent.y && loopEvent.y < ev.gesture.center.pageY + 50)) {

        //         //loopEvent.visible = false;
        //         // loopEvent.state = 'MOVING';
        //         // disableEventCreate = true;

        //         // var index = Math.random() * 2;
        //         // loopEvent.go = device.looper.commands[parseInt(index)];
        //         alert("foo");

        //         console.log("\tevent " + i);
        //         break;
        //     }
        // }

        if (!disableEventCreate) {
            disableEventCreate = true;

            var touches = ev.gesture.touches;

            // deltaX = ev.gesture.center.pageX - (screenWidth / 2);
            // deltaY = ev.gesture.center.pageY - (screenHeight / 2);
            // //angleInDegrees = Math.atan(deltaY / deltaX) * 180 / PI;
            // angleInDegrees = Math.atan2(deltaY, deltaX); // * 180 / PI;

            // x = screenWidth / 2 + (400 / 2) * Math.cos(angleInDegrees);
            // y = screenHeight / 2 + (400 / 2) * Math.sin(angleInDegrees);

            // var nearestPosition = device.processingInstance.getNearestPositionOnEventLoop(ev.gesture.center.pageX, ev.gesture.center.pageY);

            // Show behavior palette
            device.processingInstance.behaviorPalette.setPosition(ev.gesture.center.pageX, ev.gesture.center.pageY);
            device.processingInstance.behaviorPalette.show();

            // var nearestPosition = device.processingInstance.getNearestPositionOnEventLoop(ev.gesture.center.pageX, ev.gesture.center.pageY);

            // var loopEvent = new Event({
            //     x: ev.gesture.center.pageX,
            //     y: ev.gesture.center.pageY,
            //     xTarget: nearestPosition.x,
            //     yTarget: nearestPosition.y
            // });

            // loopEvent.state = 'MOVING';
            // device.processingInstance.eventLoop.events.push(loopEvent);
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

    this.scriptName = "pixel";
    // this.disableEventCreate = false;
    this.showPalette = false;
    this.font = null;
    // this.looper;

    function getLooper() {
      return this.looper;
    }
    this.getLooper = getLooper;

    function getDevice(index) {
      return this.looper.devices[index];
    }
    this.getDevice = getDevice;

    function getEventLoop(index) {
      return this.looper.devices[index].processingInstance.eventLoop;
    }
    this.getEventLoop = getEventLoop;

    function getEventSequence(index) {
      return this.looper.devices[index].processingInstance.eventLoop.events;
    }
    this.getEventSequence = getEventSequence;

    /**
     * Deploy the current state of the program to the module
     */
    function deploy() {
        console.log("Download code to the Espruino");
        // TODO: Push the current Looper state to the Espruino with an HTTP POST

        // device.looper.devices[0].processingInstance.eventLoop.events
        // var events = this.looper.devices[0].processingInstance.eventLoop.events;
        var events = this.getEventSequence(0);
        var program = '';
        for (var i = 0; i < events.length; i++) {
          program = program + getEventSequence(0)[i].behavior + '\n';
          //alert(this.looper.devices[0].processingInstance.eventLoop.events[i].behavior);
        }
        console.log(program);
        //loopEvent.behavior = behavior.script;
    }
    this.deploy = deploy;

    // TODO: Implement function to get current state of Looper's state on the Espruino

    /**
     * Processing sketch code
     */
    var deviceSketch = new Processing.Sketch(function(processing) {

        processing.currentTime = 0;
        processing.previousTime = 0;
        processing.stepFrequency = 100;

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
        // var color = generateRandomColor(255, 255, 255);
        // var backgroundColor = processing.color(color.red, color.green, color.blue);
        var backgroundColor = processing.color(240, 241, 240);

        var scriptName = "pixel";

        processing.eventLoop = new EventLoop();

        // Create behavior palette
        processing.behaviorPalette = new BehaviorPalette({
            // x: ev.gesture.center.pageX,
            // y: ev.gesture.center.pageY,
            // xTarget: nearestPosition.x,
            // yTarget: nearestPosition.y
            visible: false
        });

        // Add "default" behaviors to palette
        processing.behaviorPalette.addBehavior(0, 0, 'light', function() {
            console.log('light top level')
        });
        processing.behaviorPalette.addBehavior(100, 0, 'motion', function() {
            console.log('motion top level')
        });
        processing.behaviorPalette.addBehavior(-100, 0, 'button', function() {
            console.log('button top level')
        });
        processing.behaviorPalette.addBehavior(-200, 0, 'pin', function() {
            console.log('pin top level')
        });
        processing.behaviorPalette.addBehavior(200, 0, 'message', function() {
            console.log('message top level')
        });

        // Override setup function
        processing.setup = function() {
            processing.size(processing.screenWidth, processing.screenHeight);

            this.font = processing.loadFont("http://192.168.43.127:3000/DidactGothic.ttf");
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
                processing.rotate(-0.05 * processing.PI);
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
                    if (loopEvent.going) {
                        processing.ellipse(loopEvent.x, loopEvent.y, 70, 70);

                        // Show the program counter
                        if (loopEvent.state == 'SEQUENCED') {
                            var angle = getAngle(loopEvent.x, loopEvent.y);
                            var nearestX = processing.screenWidth / 2 + (500 / 2) * Math.cos(angle - Math.PI  / 2);
                            var nearestY = processing.screenHeight / 2 + (500 / 2) * Math.sin(angle - Math.PI  / 2);
                            processing.ellipse(nearestX, nearestY, 20, 20);
                        }
                    } else {
                        processing.ellipse(loopEvent.x, loopEvent.y, 70, 70);

                        // // Draw options for the sequenced node
                        // if (loopEvent.state == 'SEQUENCED') {
                        //     processing.ellipse(loopEvent.x + 40, loopEvent.y - 40, 30, 30);
                        // }
                    }

                    primaryFont = processing.createFont("http://192.168.43.127:3000/DidactGothic.ttf", 32);
                    processing.textFont(primaryFont, 16);
                    processing.textAlign(processing.CENTER);
                    processing.fill(65, 65, 65);
                    processing.text(loopEvent.label, loopEvent.x, loopEvent.y + 4);

                    // Calculate nearest point on circle
                    //line(loopEvent.x, loopEvent.y, screenWidth / 2, screenHeight / 2);
                }

                processing.popMatrix();
            }

            function drawBehaviorPalette() {

                processing.behaviorPalette.updatePosition();

                if (processing.behaviorPalette.visible) {

                    drawBehaviors();

                }

                function drawBehaviors() {

                    var behaviorCount = processing.behaviorPalette.behaviors.length;
                    for(var i = 0; i < behaviorCount; i++) {
                        var behavior = processing.behaviorPalette.behaviors[i];

                        processing.pushMatrix();

                        // Draw the palette
                        processing.fill(66, 214, 146);
                        processing.ellipse(processing.behaviorPalette.x + behavior.x, processing.behaviorPalette.y + behavior.y, 80, 80);

                        primaryFont = processing.createFont("http://192.168.43.127:3000/DidactGothic.ttf", 32);
                        processing.textFont(primaryFont, 16);
                        processing.textAlign(processing.CENTER);
                        processing.fill(65, 65, 65);
                        processing.text(behavior.label, processing.behaviorPalette.x + behavior.x, processing.behaviorPalette.y + behavior.y + 4);

                        processing.popMatrix();
                    }




                    // processing.pushMatrix();

                    // var eventCount = processing.eventLoop.events.length;
                    // for (var i = 0; i < eventCount; i++) {
                    //     var loopEvent = processing.eventLoop.events[i];

                    //     processing.updatePosition(loopEvent);

                    //     // Draw the event node
                    //     processing.fill(66, 214, 146);
                    //     if (loopEvent.going) {
                    //         processing.ellipse(loopEvent.x, loopEvent.y, 80, 80);
                    //     } else {
                    //         processing.ellipse(loopEvent.x, loopEvent.y, 60, 60);
                    //     }

                    //     primaryFont = processing.createFont("DidactGothic.ttf", 32);
                    //     processing.textFont(primaryFont, 16);
                    //     processing.textAlign(processing.CENTER);
                    //     processing.fill(65, 65, 65);
                    //     processing.text(loopEvent.label, loopEvent.x, loopEvent.y + 4);

                    //     // Calculate nearest point on circle
                    //     //line(loopEvent.x, loopEvent.y, screenWidth / 2, screenHeight / 2);
                    // }

                    // processing.popMatrix();
                }
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

                if (loopEvent.state === 'MOVING') {

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
                        // loopEvent.behavior = function() {
                        // loopEvent.go = function() {
                        //     // var command = loopEvent.go;
                        //     var index = Math.random() * 2;
                        //     // command = this.looper.commands[parseInt(index)];
                        //     command = "turn on";
                        //     console.log("instr: " + command);
                        // }
                        eventSequence.push({
                            event: loopEvent,
                            angle: processing.getAngle(loopEvent.x, loopEvent.y)
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

                    console.log(loopEvent);

                    var behaviorScript = loopEvent.event.behavior;
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
            processing.getAngle = getAngle;

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
            primaryFont = processing.createFont("http://192.168.43.127:3000/DidactGothic.ttf", 32);
            processing.textFont(primaryFont, 26);
            processing.textAlign(processing.CENTER);
            processing.fill(65, 65, 65);
            if (processing.eventLoop.going) {
                processing.text("stop", processing.screenWidth / 2, processing.screenHeight - 100);
            } else {
                processing.text("go", processing.screenWidth / 2, processing.screenHeight - 100);
            }

            // draw script name
            primaryFont = processing.createFont("http://192.168.43.127:3000/DidactGothic.ttf", 16);
            processing.textFont(primaryFont, 50);
            processing.textAlign(processing.CENTER);
            processing.fill(65, 65, 65);
            //processing.text(scriptName, processing.screenWidth / 2, processing.screenHeight / 7 + 20);
            processing.text(scriptName, processing.screenWidth / 2, processing.screenHeight / 2 + 10);

            // step to next node in loop
            processing.currentTime = (new Date()).getTime();
            if (processing.currentTime > (processing.previousTime + processing.stepFrequency)) {
                processing.previousTime = processing.currentTime;
                processing.eventLoop.step(); // toggle "go" and "stop"
            }

            drawEventLoop();
            drawEvents();

            drawBehaviorPalette();
        };
    });

    // deviceSketch.options.crispLines = true;
    this.processingInstance = new Processing(canvas, deviceSketch);
    this.processingInstance.canvas = canvas;
    this.processingInstance.deviceCount = deviceCount;
    // this.eventLoop = this.processingInstance.eventLoop;
    console.log(this.processingInstance);
}
