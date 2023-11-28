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
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.SideEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.composed
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.unit.Dp
import com.nydus.example.server_load_android.GameState
import com.nydus.example.server_load_android.ServerRequest
import com.nydus.example.server_load_android.ServerRequestType
import java.time.Instant

@Composable
fun PriorityScreen() {
    LaunchedEffect(LocalLifecycleOwner.current){
        GameState.requestQueue.add(ServerRequest(ServerRequestType.update, Instant.now(), GameState::Update))
    }
    Column(
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.fillMaxSize()
    ) {
        Box(modifier = Modifier.fillMaxWidth()) {
            Text(text = "Building priority")
            Slider(
                value = GameState.BuildingPriority.floatValue % 1f,
                modifier = Modifier.offset(x = Dp(0f), y = Dp(10f)),
                onValueChange = {
                    GameState.BuildingPriority.floatValue = it
                })
        }
        Box(modifier = Modifier.fillMaxWidth()) {
            Text(text = "Research priority")
            Slider(
                value = GameState.ResearchPriority.floatValue % 1f,
                modifier = Modifier.offset(x = Dp(0f), y = Dp(10f)),
                onValueChange = {
                    GameState.ResearchPriority.floatValue = it
                })
        }
        Box(modifier = Modifier.fillMaxWidth()) {
            Text(text = "Boost priority")
            Slider(
                value = GameState.BoostPriority.floatValue % 1f,
                modifier = Modifier.offset(x = Dp(0f), y = Dp(10f)),
                onValueChange = {
                    GameState.BoostPriority.floatValue = it
                })
        }
        FilledIconToggleButton(
            checked = GameState.BoostPriority.floatValue > 1f,
            modifier = Modifier.width(
                Dp(300f)
            ),
            onCheckedChange = { if (GameState.BoostPriority.floatValue > 1f){
                GameState.BoostPriority.floatValue -= 1f
                GameState.BuildingPriority.floatValue -= 1f
                GameState.ResearchPriority.floatValue -= 1f
            } else {
                GameState.BoostPriority.floatValue += 1f
                GameState.BuildingPriority.floatValue += 1f
                GameState.ResearchPriority.floatValue += 1f
            }
            }
        ) {
            Text(text = "Dynamic priority")
        }
        FilledIconToggleButton(
            checked = GameState.FocusedBuilding.intValue == -2,
            modifier = Modifier.width(Dp(300f)),
            onCheckedChange = { 
                if (GameState.FocusedBuilding.intValue == -2){
                    GameState.FocusedBuilding.intValue = 0
                }
                else {
                    GameState.FocusedBuilding.intValue = -2
                }
            }
        ) {
            Text(text = "Dynamic building focus")
        }
        FilledIconToggleButton(
            checked = GameState.FocusedResearch.intValue == -2,
            modifier = Modifier.width(Dp(300f)),
            onCheckedChange = { 
                if (GameState.FocusedResearch.intValue == -2){
                    GameState.FocusedResearch.intValue = 0
                }
                else {
                    GameState.FocusedResearch.intValue = -2
                }
            }
        ) {
            Text(text = "Dynamic research focus")
        }
    }
}