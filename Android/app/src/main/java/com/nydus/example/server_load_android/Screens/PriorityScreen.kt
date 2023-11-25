package com.nydus.example.server_load_android.Screens

import android.widget.GridLayout
import android.widget.ToggleButton
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.IntrinsicSize
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Button
import androidx.compose.material3.FilledIconToggleButton
import androidx.compose.material3.FilledTonalIconToggleButton
import androidx.compose.material3.Slider
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.composed
import androidx.compose.ui.unit.Dp

@Composable
fun PriorityScreen() {
    var building_priority by remember { mutableFloatStateOf(0.5f) }
    var research_priority by remember { mutableFloatStateOf(0.5f) }
    var boost_priority by remember { mutableFloatStateOf(0.5f) }
    var dynamic_priority by remember {
        mutableStateOf(false)
    }
    var dynamic_building_focus by remember {
        mutableStateOf(false)
    }
    var dynamic_research_focus by remember {
        mutableStateOf(false)
    }
    Column(
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.fillMaxSize()
    ) {
        Box(modifier = Modifier.fillMaxWidth()) {
            Text(text = "Building priority")
            Slider(
                value = building_priority,
                modifier = Modifier.offset(x = Dp(0f), y = Dp(10f)),
                onValueChange = {
                    building_priority = it
                    dynamic_priority = false
                })
        }
        Box(modifier = Modifier.fillMaxWidth()) {
            Text(text = "Research priority")
            Slider(
                value = research_priority,
                modifier = Modifier.offset(x = Dp(0f), y = Dp(10f)),
                onValueChange = {
                    research_priority = it
                    dynamic_priority = false
                })
        }
        Box(modifier = Modifier.fillMaxWidth()) {
            Text(text = "Boost priority")
            Slider(
                value = boost_priority,
                modifier = Modifier.offset(x = Dp(0f), y = Dp(10f)),
                onValueChange = {
                    boost_priority = it
                    dynamic_priority = false
                })
        }
        FilledIconToggleButton(
            checked = dynamic_priority,
            modifier = Modifier.width(
                Dp(300f)
            ),
            onCheckedChange = { dynamic_priority = !dynamic_priority }
        ) {
            Text(text = "Dynamic priority")
        }
        FilledIconToggleButton(
            checked = dynamic_building_focus,
            modifier = Modifier.width(Dp(300f)),
            onCheckedChange = { dynamic_building_focus = !dynamic_building_focus }
        ) {
            Text(text = "Dynamic building focus")
        }
        FilledIconToggleButton(
            checked = dynamic_research_focus,
            modifier = Modifier.width(Dp(300f)),
            onCheckedChange = { dynamic_research_focus = !dynamic_research_focus }
        ) {
            Text(text = "Dynamic research focus")
        }
    }
}