
import QtQuick 2.15

Item {
    id: root
    property real speed: 0
    property real maxSpeed: 70
    anchors.fill: parent

    Canvas {
        id: leftSpeedCanvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            var w = width;
            var h = height;
            var cx = w/2;
            var cy = h/2;
            var radius = Math.min(w,h) * 0.45;

            ctx.reset();

            // background radial gradient
            var g = ctx.createRadialGradient(cx - radius*0.2, cy - radius*0.3, radius*0.1, cx, cy, radius*1.1);
            g.addColorStop(0, '#111111');
            g.addColorStop(0.6, '#222222');
            g.addColorStop(1, '#000000');
            ctx.fillStyle = g;
            ctx.beginPath();
            ctx.arc(cx, cy, radius*1.05, 0, Math.PI*2);
            ctx.fill();

            // outer bezel
            ctx.lineWidth = Math.max(2, radius*0.03);
            ctx.strokeStyle = '#1a1a1a';
            ctx.beginPath();
            ctx.arc(cx, cy, radius*1.02, 0, Math.PI*2);
            ctx.stroke();

            // ticks and numbers
            // make the left-most tick correspond to 0 degrees (pointing left)
            var startAngle = Math.PI; // 180 degrees -> left-most
            var endAngle = 0; // 0 degrees -> right-most
            var span = startAngle - endAngle;

            ctx.textAlign = 'center';
            ctx.textBaseline = 'middle';
            ctx.font = Math.round(radius*0.12) + 'px sans-serif';
            ctx.fillStyle = '#d3d7cf';

            var majorStep = 10; // label every 10 km/h
            var numSteps = root.maxSpeed / 5; // minor steps per 5 km/h
            for (var i=0;i<=numSteps;i++) {
                var t = i / numSteps;
                var angle = startAngle + t * span;
                var inner = radius*0.82;
                var outer = radius*0.95;
                if (i % (majorStep/5) === 0) {
                    inner = radius*0.76;
                    ctx.lineWidth = Math.max(1, radius*0.008);
                } else {
                    ctx.lineWidth = Math.max(1, radius*0.004);
                }
                var x1 = cx + inner * Math.cos(angle);
                var y1 = cy + inner * Math.sin(angle);
                var x2 = cx + outer * Math.cos(angle);
                var y2 = cy + outer * Math.sin(angle);
                ctx.beginPath();
                ctx.moveTo(x1,y1);
                ctx.lineTo(x2,y2);
                ctx.strokeStyle = '#cfcfcf';
                ctx.stroke();

                // draw numeric labels every majorStep
                var value = t * root.maxSpeed;
                if (Math.round(value) % majorStep === 0) {
                    var tx = cx + (radius*0.62) * Math.cos(angle);
                    var ty = cy + (radius*0.62) * Math.sin(angle);
                    ctx.fillStyle = '#dcdcdc';
                    ctx.font = Math.round(radius*0.09) + 'px sans-serif';
                    ctx.fillText(Math.round(value), tx, ty);
                }
            }

            // center shadow/inner circle
            ctx.beginPath();
            ctx.arc(cx, cy, radius*0.55, 0, Math.PI*2);
            var g2 = ctx.createRadialGradient(cx - radius*0.1, cy - radius*0.1, radius*0.05, cx, cy, radius*0.7);
            g2.addColorStop(0, 'rgba(255,255,255,0.05)');
            g2.addColorStop(1, 'rgba(0,0,0,0.6)');
            ctx.fillStyle = g2;
            ctx.fill();

            // needle
            var angleForSpeed = startAngle + (root.speed / root.maxSpeed) * span;
            var needleLen = radius*0.90;

            // needle shadow
            ctx.beginPath();
            ctx.strokeStyle = 'rgba(0,0,0,0.6)';
            ctx.lineWidth = Math.max(6, radius*0.06);
            ctx.lineCap = 'round';
            ctx.moveTo(cx + -2 + (needleLen*0.05) * Math.cos(angleForSpeed - Math.PI/2), cy + -2 + (needleLen*0.05) * Math.sin(angleForSpeed - Math.PI/2));
            ctx.lineTo(cx + -2 + needleLen * Math.cos(angleForSpeed), cy + -2 + needleLen * Math.sin(angleForSpeed));
            ctx.stroke();

            // needle body
            ctx.beginPath();
            var grad = ctx.createLinearGradient(cx, cy, cx + needleLen * Math.cos(angleForSpeed), cy + needleLen * Math.sin(angleForSpeed));
            grad.addColorStop(0, '#7f0000');
            grad.addColorStop(1, '#ff0000');
            ctx.strokeStyle = grad;
            ctx.lineWidth = Math.max(4, radius*0.05);
            ctx.lineCap = 'round';
            ctx.moveTo(cx + (needleLen*0.06) * Math.cos(angleForSpeed + Math.PI/2), cy + (needleLen*0.06) * Math.sin(angleForSpeed + Math.PI/2));
            ctx.lineTo(cx + needleLen * Math.cos(angleForSpeed), cy + needleLen * Math.sin(angleForSpeed));
            ctx.stroke();

            // needle cap
            ctx.beginPath();
            ctx.fillStyle = '#222222';
            ctx.arc(cx, cy, Math.max(6, radius*0.07), 0, Math.PI*2);
            ctx.fill();
            ctx.lineWidth = 2;
            ctx.strokeStyle = '#333333';
            ctx.stroke();

            // central speed text - move upward so number appears above center
            ctx.fillStyle = '#ffffff';
            ctx.font = Math.round(radius*0.30) + 'px sans-serif';
            //ctx.fillText(Math.round(root.speed), cx, cy - radius*0.12);
            ctx.fillText(Math.round(root.speed), cx, cy + 40);

            // unit text - place below the number but still near center
            ctx.fillStyle = '#cfcfcf';
            ctx.font = Math.round(radius*0.09) + 'px sans-serif';
            //ctx.fillText('m/s', cx, cy + radius*0.08);
            ctx.fillText('Left Wheel RPM', cx, cy + 55);
        }
    }

    // repaint whenever speed or size changes
    onSpeedChanged: leftSpeedCanvas.requestPaint()
    onWidthChanged: leftSpeedCanvas.requestPaint()
    onHeightChanged: leftSpeedCanvas.requestPaint()

    Component.onCompleted: leftSpeedCanvas.requestPaint()
}
