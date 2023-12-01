package com.nydus.example.server_load_android.Screens

import android.graphics.Paint.Align
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material3.Button
import androidx.compose.material3.FilledIconToggleButton
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.modifier.modifierLocalMapOf
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.unit.Dp
import com.nydus.example.server_load_android.Constants
import com.nydus.example.server_load_android.GameState
import com.nydus.example.server_load_android.ServerRequest
import com.nydus.example.server_load_android.ServerRequestType
import java.time.Instant

@Composable
fun BuildingScreen() {
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
        Constants.BuildingItems.forEach {
            val localBuild = it.checkNum
            Box(
                modifier = Modifier
                    .width(Dp(300f))
                    .clip(
                        CircleShape
                    )
                    .background(MaterialTheme.colorScheme.secondaryContainer)
            ) {
                Text(
                    text = "${it.label}: ${it.reference.floatValue}",
                    modifier = Modifier
                        .align(Alignment.CenterStart)
                        .offset(x = Dp(10f), y = Dp(0f))
                )
                FilledIconToggleButton(
                    checked = GameState.FocusedBuilding.intValue == localBuild,
                    modifier = Modifier.align(Alignment.CenterEnd),
                    onCheckedChange = {
                        GameState.FocusedBuilding.intValue = localBuild
                        GameState.FocusBuildingRequestInProgress = true
                        GameState.requestQueue.add(ServerRequest(ServerRequestType.setFocusedBuilding, Instant.now(), GameState::setFocusedBuilding))
                    }) {
                    Icon(imageVector = Icons.Default.CheckCircle, contentDescription = "check")
                }
            }
        }
        
    }
}