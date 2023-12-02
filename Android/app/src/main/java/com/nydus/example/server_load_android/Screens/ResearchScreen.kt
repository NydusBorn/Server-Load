package com.nydus.example.server_load_android.Screens

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material3.FilledIconToggleButton
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.draw.clip
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.unit.Dp
import com.nydus.example.server_load_android.Constants
import com.nydus.example.server_load_android.GameState
import com.nydus.example.server_load_android.ServerRequest
import com.nydus.example.server_load_android.ServerRequestType
import java.time.Instant

@Composable
fun ResearchScreen(){
    LaunchedEffect(LocalLifecycleOwner.current){
        GameState.requestQueue.add(ServerRequest(ServerRequestType.update, Instant.now(), GameState::update))
    }
    
    Column(
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.fillMaxSize().verticalScroll(rememberScrollState())
    ) {
        Spacer(modifier = Modifier.height(Dp(10f)))
        Constants.ResearchItems.forEach {
            val localResearch = it.checkNum
            Box(
                modifier = Modifier
                    .width(Dp(300f))
                    .clip(
                        CircleShape
                    )
                    .background(if (GameState.Overflows.intValue >= it.overflowRequirement && it.requirementReference.floatValue >= it.valueRequirement) MaterialTheme.colorScheme.secondaryContainer else MaterialTheme.colorScheme.background).alpha(if (GameState.Overflows.intValue >= it.overflowRequirement && it.requirementReference.floatValue >= it.valueRequirement) 1f else 0f)
            ) {
                Text(
                    text = "${it.label}: ${it.reference.floatValue}",
                    modifier = Modifier
                        .align(Alignment.CenterStart)
                        .offset(x = Dp(10f), y = Dp(0f))
                )
                FilledIconToggleButton(
                    checked = GameState.FocusedResearch.intValue == localResearch,
                    modifier = Modifier.align(Alignment.CenterEnd),
                    enabled = GameState.Overflows.intValue >= it.overflowRequirement && it.requirementReference.floatValue >= it.valueRequirement,
                    onCheckedChange = {
                        GameState.FocusedResearch.intValue = localResearch
                        GameState.FocusResearchRequestInProgress = true
                        GameState.requestQueue.add(ServerRequest(ServerRequestType.setFocusedResearch, Instant.now(), GameState::setFocusedResearch))
                    }) {
                    Icon(imageVector = Icons.Default.CheckCircle, contentDescription = "check")
                }
            }
            Spacer(modifier = Modifier.height(Dp(10f)))
        }
    }
}