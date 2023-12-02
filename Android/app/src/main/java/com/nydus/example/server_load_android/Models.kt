package com.nydus.example.server_load_android

import androidx.compose.runtime.MutableFloatState
import androidx.compose.ui.graphics.vector.ImageVector
import java.lang.ref.Reference

data class navItem(
    val label: String,
    val icon: ImageVector,
    val route: String
)

data class Building(
    var label: String,
    val checkNum: Int,
    val reference: MutableFloatState,
    val overflowRequirement: Int,
    val valueRequirement: Float,
    val requirementReference: MutableFloatState
)

data class Research(
    var label: String,
    val checkNum: Int,
    val reference: MutableFloatState,
    val overflowRequirement: Int,
    val valueRequirement: Float,
    val requirementReference: MutableFloatState
) 