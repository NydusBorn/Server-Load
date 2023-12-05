package com.nydus.example.server_load_android.Screens

import android.app.AlertDialog
import android.graphics.Paint.Align
import android.support.v4.os.IResultReceiver._Parcel
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
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
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.modifier.modifierLocalMapOf
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.Dp
import com.nydus.example.server_load_android.Connector
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
    Box(Modifier.fillMaxSize()){
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
                        .background(if ((GameState.Overflows.intValue >= it.overflowRequirement && it.requirementReference.floatValue >= it.valueRequirement) || (GameState.Overflows.intValue >= it.overflowRequirement && GameState.Overflows.intValue >= 4)) MaterialTheme.colorScheme.secondaryContainer else MaterialTheme.colorScheme.background).alpha(if ((GameState.Overflows.intValue >= it.overflowRequirement && it.requirementReference.floatValue >= it.valueRequirement) || (GameState.Overflows.intValue >= it.overflowRequirement && GameState.Overflows.intValue >= 4)) 1f else 0f)
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
                        enabled = GameState.Overflows.intValue >= it.overflowRequirement && it.requirementReference.floatValue >= it.valueRequirement,
                        onCheckedChange = {
                            GameState.FocusedBuilding.intValue = localBuild
                            GameState.FocusBuildingRequestInProgress = true
                            GameState.requestQueue.add(ServerRequest(ServerRequestType.setFocusedBuilding, Instant.now(), GameState::setFocusedBuilding))
                        }) {
                        Icon(imageVector = Icons.Default.CheckCircle, contentDescription = "check")
                    }
                }
                Spacer(modifier = Modifier.height(Dp(10f)))
            }
        }
        Column (
            verticalArrangement = Arrangement.Bottom,
            horizontalAlignment = Alignment.CenterHorizontally,
            modifier = Modifier.fillMaxSize()
        ){
            Button(
                enabled = showOverflowButtonCondtition(GameState.Overflows.intValue),
                modifier = Modifier.alpha(if (showOverflowButtonCondtition(GameState.Overflows.intValue)) 1f else 0f),
                onClick = { 
                    GameState.OverflowRequestInProgress = true
                    GameState.requestQueue.add(ServerRequest(ServerRequestType.overflow, Instant.now(), GameState::overflow))
                    AlertDialog.Builder(Connector.NavController!!.context).setTitle("Progress")
                        .setMessage(
                            when (GameState.Overflows.intValue){
                                0 -> "Bits can have a multiplier now, you can start building bytes."
                                1 -> "Bytes can have a multiplier now, you can start building Kilo packers, you now have access to dynamic priority."
                                2 -> "Kilo packers can have a multiplier now, you can start building Mega packers."
                                3 -> "Mega packers can have a multiplier now, you can start building Giga packers, you now have access to dynamic building focus, and you can increase your process count."
                                4 -> "Giga packers can have a multiplier now, you can start building Tera packers, and your processes can have a multiplier."
                                5 -> "Tera packers can have a multiplier now, you can start building Peta packers, you now have access to dynamic research focus."
                                6 -> "Peta packers can have a multiplier now, you can start building Exa packers."
                                else -> "None"
                            }
                        )
                        .show()
                }) {
                Text(text = "Overflow")
            }
            Spacer(modifier = Modifier
                .height(Dp(20f)))
        }
        
    }
}

fun showOverflowButtonCondtition(overflowCount:Int):Boolean{
    when(overflowCount){
        0 -> if (GameState.Building_Bits.floatValue >= 8f) return true
        1 -> if (GameState.Building_Bytes.floatValue >= 1024f) return true
        2 -> if (GameState.Building_Kilo.floatValue >= 1024f) return true
        3 -> if (GameState.Building_Mega.floatValue >= 1024f) return true
        4 -> if (GameState.Building_Giga.floatValue >= 1024f) return true
        5 -> if (GameState.Building_Tera.floatValue >= 1024f) return true
        6 -> if (GameState.Building_Peta.floatValue >= 1024f) return true
    }
    return false
}