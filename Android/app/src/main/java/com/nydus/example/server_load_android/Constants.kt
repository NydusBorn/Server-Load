package com.nydus.example.server_load_android

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.List
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.filled.Settings

object Constants {
    val BottomNavItems = listOf(
        navItem(label = "Priorities", icon = Icons.Default.List, route = "Priority"),
        navItem(label = "Buildings", icon = Icons.Default.Home, route = "Building"),
        navItem(label = "Research", icon = Icons.Default.Search, route = "Research"),
        navItem(label = "Settings", icon = Icons.Default.Settings, route = "Setting")
    )
}