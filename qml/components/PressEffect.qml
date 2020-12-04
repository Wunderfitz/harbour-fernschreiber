import QtQuick 2.0
import Sailfish.Silica 1.0

ShaderEffect {
    property variant source
    property color color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
    fragmentShader: "
        uniform sampler2D source;
        uniform highp vec4 color;
        uniform lowp float qt_Opacity;
        varying highp vec2 qt_TexCoord0;
        void main(void) {
            highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
            gl_FragColor = vec4(mix(pixelColor.rgb/max(pixelColor.a, 0.00390625), color.rgb/max(color.a, 0.00390625), color.a) * pixelColor.a, pixelColor.a) * qt_Opacity;
        }"
}
