package com.nydus.example.server_load_android

import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.navigation.NavController
import androidx.navigation.NavHost

object Connector{
    var BottomNavBarVisibility = mutableFloatStateOf(0f)
    
    var navHost: NavController? = null
}
