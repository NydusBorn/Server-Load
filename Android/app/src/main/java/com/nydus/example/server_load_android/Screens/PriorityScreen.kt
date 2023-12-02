package com.nydus.example.server_load_android.Screens

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.width
import androidx.compose.material3.FilledIconToggleButton
import androidx.compose.material3.Slider
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.unit.Dp
import com.nydus.example.server_load_android.GameState
import com.nydus.example.server_load_android.ServerRequest
import com.nydus.example.server_load_android.ServerRequestType
import java.time.Instant

@Composable
fun PriorityScreen() {
    LaunchedEffect(LocalLifecycleOwner.current) {
        GameState.requestQueue.add(
            ServerRequest(
                ServerRequestType.update,
                Instant.now(),
                GameState::update
            )
        )
    }
    Column(
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.fillMaxSize()
    ) {
        Box(modifier = Modifier.fillMaxWidth()) {
            Text(text = "Building priority")
            Slider(
                value = GameState.BuildingPriority.floatValue,
                modifier = Modifier.offset(x = Dp(0f), y = Dp(10f)),
                onValueChange = {
                    if (GameState.DynamicPriority.value) return@Slider
                    var total = 1f
                    GameState.BuildingPriority.floatValue = it
                    total -= GameState.BuildingPriority.floatValue
                    if (GameState.ResearchPriority.floatValue > GameState.BoostPriority.floatValue) {
                        total -= GameState.BoostPriority.floatValue
                        GameState.ResearchPriority.floatValue = total
                    } else {
                        total -= GameState.ResearchPriority.floatValue
                        GameState.BoostPriority.floatValue = total
                    }
                    GameState.PriorityRequestInProgress = true
                    GameState.requestQueue.add(
                        ServerRequest(
                            ServerRequestType.setPriority,
                            Instant.now(),
                            GameState::setPriority
                        )
                    )
                })
        }
        Box(modifier = Modifier.fillMaxWidth()) {
            Text(text = "Research priority")
            Slider(
                value = GameState.ResearchPriority.floatValue,
                modifier = Modifier.offset(x = Dp(0f), y = Dp(10f)),
                onValueChange = {
                    if (GameState.DynamicPriority.value) return@Slider
                    var total = 1f
                    GameState.ResearchPriority.floatValue = it
                    total -= GameState.ResearchPriority.floatValue
                    if (GameState.BuildingPriority.floatValue > GameState.BoostPriority.floatValue) {
                        total -= GameState.BoostPriority.floatValue
                        GameState.BuildingPriority.floatValue = total
                    } else {
                        total -= GameState.BuildingPriority.floatValue
                        GameState.BoostPriority.floatValue = total
                    }
                    GameState.PriorityRequestInProgress = true
                    GameState.requestQueue.add(
                        ServerRequest(
                            ServerRequestType.setPriority,
                            Instant.now(),
                            GameState::setPriority
                        )
                    )
                })
        }
        Box(modifier = Modifier.fillMaxWidth()) {
            Text(text = "Boost priority")
            Slider(
                value = GameState.BoostPriority.floatValue,
                modifier = Modifier.offset(x = Dp(0f), y = Dp(10f)),
                onValueChange = {
                    if (GameState.DynamicPriority.value) return@Slider
                    var total = 1f
                    GameState.BoostPriority.floatValue = it
                    total -= GameState.BoostPriority.floatValue
                    if (GameState.ResearchPriority.floatValue > GameState.BuildingPriority.floatValue) {
                        total -= GameState.BuildingPriority.floatValue
                        GameState.ResearchPriority.floatValue = total
                    } else {
                        total -= GameState.ResearchPriority.floatValue
                        GameState.BuildingPriority.floatValue = total
                    }
                    GameState.PriorityRequestInProgress = true
                    GameState.requestQueue.add(
                        ServerRequest(
                            ServerRequestType.setPriority,
                            Instant.now(),
                            GameState::setPriority
                        )
                    )
                })
        }
        FilledIconToggleButton(
            checked = GameState.DynamicPriority.value,
            modifier = Modifier.width(
                Dp(300f)
            ).alpha(if (GameState.Overflows.intValue >= 2) 1f else 0f),
            enabled = GameState.Overflows.intValue >= 2,
            onCheckedChange = {
                GameState.DynamicPriority.value = !GameState.DynamicPriority.value
                GameState.PriorityRequestInProgress = true
                GameState.requestQueue.add(
                    ServerRequest(
                        ServerRequestType.setPriority,
                        Instant.now(),
                        GameState::setPriority
                    )
                )
            }
        ) {
            Text(text = "Dynamic priority")
        }
        FilledIconToggleButton(
            checked = GameState.FocusedBuilding.intValue == -2,
            modifier = Modifier.width(Dp(300f)).alpha(if (GameState.Overflows.intValue >= 4) 1f else 0f),
            enabled = GameState.Overflows.intValue >= 4,
            onCheckedChange = {
                if (GameState.FocusedBuilding.intValue == -2) {
                    GameState.FocusedBuilding.intValue = 0
                } else {
                    GameState.FocusedBuilding.intValue = -2
                }
                GameState.FocusBuildingRequestInProgress = true
                GameState.requestQueue.add(
                    ServerRequest(
                        ServerRequestType.setFocusedBuilding,
                        Instant.now(),
                        GameState::setFocusedBuilding
                    )
                )
            }
        ) {
            Text(text = "Dynamic building focus")
        }
        FilledIconToggleButton(
            checked = GameState.FocusedResearch.intValue == -2,
            modifier = Modifier.width(Dp(300f)).alpha(if (GameState.Overflows.intValue >= 6) 1f else 0f),
            enabled = GameState.Overflows.intValue >= 6,
            onCheckedChange = {
                if (GameState.FocusedResearch.intValue == -2) {
                    GameState.FocusedResearch.intValue = 0
                } else {
                    GameState.FocusedResearch.intValue = -2
                }
                GameState.FocusResearchRequestInProgress = true
                GameState.requestQueue.add(
                    ServerRequest(
                        ServerRequestType.setFocusedResearch,
                        Instant.now(),
                        GameState::setFocusedResearch
                    )
                )
            }
        ) {
            Text(text = "Dynamic research focus")
        }
    }
}