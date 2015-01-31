#include "climb.h"

const char* base_tci = 
STR(threads=8)
STR(thread_priority=low)
STR(warrior="Warrior_Fury_2h_T17M")
STR(level=100)
STR(role=attack)
STR(position=back)
STR(talents=1321321)
STR(glyphs=unending_rage/raging_wind/heroic_leap)
STR(spec=fury)
STR(actions.precombat=flask,type=greater_draenic_strength_flask)
STR(actions.precombat+=/food,type=blackrock_barbecue)
STR(actions.precombat+=/stance,choose=battle)
STR(actions.precombat+=/snapshot_stats)
STR(actions.precombat+=/potion,name=draenic_strength)
STR(actions=charge)
STR(actions+=/auto_attack)
STR(actions+=/berserker_rage,if=buff.enrage.down|(talent.unquenchable_thirst.enabled&buff.raging_blow.down))
STR(actions+=/heroic_leap,if=(raid_event.movement.distance>25&raid_event.movement.in>45)|!raid_event.movement.exists)
STR(actions+=/potion,name=draenic_strength,if=(target.health.pct<20&buff.recklessness.up)|target.time_to_die<=25)
STR(actions+=/call_action_list,name=single_target,if=(raid_event.adds.cooldown<60&raid_event.adds.count>2&active_enemies=1)|raid_event.movement.cooldown<5)
STR(actions+=/recklessness,if=((target.time_to_die>190|target.health.pct<20)&(buff.bloodbath.up|!talent.bloodbath.enabled))|target.time_to_die<=12|talent.anger_management.enabled)
STR(actions+=/avatar,if=(buff.recklessness.up|target.time_to_die<=30))
STR(actions+=/blood_fury,if=buff.bloodbath.up|!talent.bloodbath.enabled|buff.recklessness.up)
STR(actions+=/berserking,if=buff.bloodbath.up|!talent.bloodbath.enabled|buff.recklessness.up)
STR(actions+=/arcane_torrent,if=rage<rage.max-40)
STR(actions+=/call_action_list,name=single_target,if=active_enemies=1)
STR(actions.single_target=bloodbath)
STR(actions.single_target+=/recklessness,if=target.health.pct<20&raid_event.adds.exists)
STR(actions.single_target+=/wild_strike,if=rage>110&target.health.pct>20)
STR(actions.single_target+=/bloodthirst,if=(!talent.unquenchable_thirst.enabled&rage<80)|buff.enrage.down)
STR(actions.single_target+=/ravager,if=buff.bloodbath.up|(!talent.bloodbath.enabled&(!raid_event.adds.exists|raid_event.adds.cooldown>60|target.time_to_die<40)))
STR(actions.single_target+=/execute,if=buff.sudden_death.react)
STR(actions.single_target+=/siegebreaker)
STR(actions.single_target+=/storm_bolt)
STR(actions.single_target+=/wild_strike,if=buff.bloodsurge.up)
STR(actions.single_target+=/execute,if=buff.enrage.up|target.time_to_die<12)
STR(actions.single_target+=/dragon_roar,if=buff.bloodbath.up|!talent.bloodbath.enabled)
STR(actions.single_target+=/raging_blow)
STR(actions.single_target+=/wild_strike,if=buff.enrage.up&target.health.pct>20)
STR(actions.single_target+=/bladestorm,if=!raid_event.adds.exists)
STR(actions.single_target+=/shockwave,if=!talent.unquenchable_thirst.enabled)
STR(actions.single_target+=/impending_victory,if=!talent.unquenchable_thirst.enabled&target.health.pct>20)
STR(actions.single_target+=/bloodthirst)
STR(head=blackhands_faceguard,id=115584,bonus_id=567)
STR(neck=vertebrae_protector,id=113900,bonus_id=567,enchant=gift_of_critical_strike)
STR(shoulders=overdriven_spaulders,id=113990,bonus_id=567)
STR(back=charbreath_firecloak,id=113916,bonus_id=567,enchant=gift_of_critical_strike)
STR(chest=blackhands_chestguard,id=115582,bonus_id=567)
STR(wrists=bracers_of_visceral_force,id=119331,bonus_id=567)
STR(hands=blackhands_handguards,id=115583,bonus_id=567)
STR(waist=ironcrushers_collar,id=113950,bonus_id=567)
STR(legs=blackhands_legguards,id=115580,bonus_id=567)
STR(feet=iron_bellow_sabatons,id=113961,bonus_id=567)
STR(finger1=spellbound_runic_band_of_elemental_power,id=118305,enchant=gift_of_critical_strike)
STR(finger2=seal_of_the_savage_howl,id=113940,bonus_id=567,enchant=gift_of_critical_strike)
STR(trinket1=horn_of_screaming_spirits,id=119193,bonus_id=567)
STR(trinket2=forgemasters_insignia,id=113983,bonus_id=567)
STR(main_hand=crescent_of_living_magma,id=113886,bonus_id=567,enchant=mark_of_the_thunderlord)
STR(off_hand=crescent_of_living_magma,id=113886,bonus_id=567,enchant=mark_of_the_thunderlord)
;