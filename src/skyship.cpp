/****************************************************************************
 *                   ^     +----- |  / ^     ^ |     | +-\                  *
 *                  / \    |      | /  |\   /| |     | |  \                 *
 *                 /   \   +---   |<   | \ / | |     | |  |                 *
 *                /-----\  |      | \  |  v  | |     | |  /                 *
 *               /       \ |      |  \ |     | +-----+ +-/                  *
 ****************************************************************************
 * AFKMud Copyright 1997-2008 by Roger Libiez (Samson),                     *
 * Levi Beckerson (Whir), Michael Ward (Tarl), Erik Wolfe (Dwip),           *
 * Cameron Carroll (Cam), Cyberfox, Karangi, Rathian, Raine,                *
 * Xorith, and Adjani.                                                      *
 * All Rights Reserved.                                                     *
 * Registered with the United States Copyright Office: TX 5-877-286         *
 *                                                                          *
 * External contributions from Remcon, Quixadhal, Zarius, and many others.  *
 *                                                                          *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,        *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,        *
 * Grishnakh, Fireblade, and Nivek.                                         *
 *                                                                          *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                      *
 *                                                                          *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,      *
 * Michael Seifert, and Sebastian Hammer.                                   *
 ****************************************************************************
 *  Skyship Module for Overland - based off of Ymris' Dragonflight Module   *
 *        Original Dragonflight Module written by Ymris of Terahoun         *
 ****************************************************************************/

#include "mud.h"
#include "event.h"
#include "mobindex.h"
#include "overland.h"
#include "skyship.h"

list<landing_data*> landinglist;

/*
 * Remove a skyship when it is no longer needed
 */
void purge_skyship( char_data * ch, char_data * skyship )
{
   ch->print_room( "The skyship pilot ascends and takes to the wind.\r\n" );
   skyship->mx = 0;
   skyship->my = 0;

   /*
    * Release the player from the skyship 
    */
   ch->unset_pcflag( PCFLAG_BOARDED );
   ch->has_skyship = false;
   ch->my_skyship = NULL;

   /*
    * After this short timer runs out, it'll be extracted properly 
    */
   skyship->timer = 1;
   cancel_event( ev_skyship, ch );
   return;
}

/*
 * Skyship landing function
 */
void land_skyship( char_data * ch, char_data * skyship, bool arrived )
{
   if( !ch->isnpc(  ) && arrived )
   {
      ch->mx = skyship->dcoordx;
      ch->my = skyship->dcoordy;
      ch->inflight = false;
      cancel_event( ev_skyship, ch );
   }

   skyship->map = skyship->my_rider->map;
   skyship->mx = skyship->my_rider->mx;
   skyship->my = skyship->my_rider->my;
   skyship->backtracking = false;
   skyship->inflight = false;

   if( skyship->has_actflag( ACT_BOARDED ) )
   {
      interpret( ch, "look" );
      skyship->print_room( "&CThe skyship descends and lands on the platform.\r\n" );
      skyship->print_room( "Everyone aboard the ship disembarks.\r\n" );
      purge_skyship( ch, skyship );
      return;
   }
   else
   {
      skyship->print_room( "&CA skyship descends from above and lands on the platform.\r\n" );
      return;
   }
}

/*
 * Skyship flight function
 */
void fly_skyship( char_data * ch, char_data * skyship )
{
   char_data *pair;
   double dist, angle;
   int speed = 10;   /* Speed of the skyships on the overland */

   /*
    * Reset the boredom counter 
    */
   skyship->zzzzz = 0;

   if( ch && skyship->has_actflag( ACT_BOARDED ) )
      pair = ch;
   else
   {
      pair = skyship;
      speed *= 5;
   }

   /*
    * If skyship is close to the landing site... 
    */
   if( ( ( skyship->my - skyship->dcoordy ) <= speed )
       && ( skyship->my - skyship->dcoordy ) >= -speed
       && ( ( skyship->mx - skyship->dcoordx ) <= speed ) && ( skyship->mx - skyship->dcoordx ) >= -speed )
   {
      land_skyship( pair, skyship, true );
      return;
   }

   /*
    * up up and away 
    */
   dist = distance( skyship->mx, skyship->my, skyship->dcoordx, skyship->dcoordy );
   angle = calc_angle( skyship->mx, skyship->my, skyship->dcoordx, skyship->dcoordy, &dist );

   if( angle == -1 )
      skyship->heading = -1;
   else if( angle >= 360 )
      skyship->heading = DIR_NORTH;
   else if( angle >= 315 )
      skyship->heading = DIR_NORTHWEST;
   else if( angle >= 270 )
      skyship->heading = DIR_WEST;
   else if( angle >= 225 )
      skyship->heading = DIR_SOUTHWEST;
   else if( angle >= 180 )
      skyship->heading = DIR_SOUTH;
   else if( angle >= 135 )
      skyship->heading = DIR_SOUTHEAST;
   else if( angle >= 90 )
      skyship->heading = DIR_EAST;
   else if( angle >= 45 )
      skyship->heading = DIR_NORTHEAST;
   else if( angle >= 0 )
      skyship->heading = DIR_NORTH;

   /*
    * move towards dest in steps of "speed" rooms  (salt to taste) 
    */
   switch ( skyship->heading )
   {
      case DIR_NORTH:
         pair->my = pair->my - speed;
         if( pair == ch )
            skyship->my = skyship->my - speed;
         break;

      case DIR_EAST:
         pair->mx = pair->mx + speed;
         if( pair == ch )
            skyship->mx = skyship->mx + speed;
         break;

      case DIR_SOUTH:
         pair->my = pair->my + speed;
         if( pair == ch )
            skyship->my = skyship->my + speed;
         break;

      case DIR_WEST:
         pair->mx = pair->mx - speed;
         if( pair == ch )
            skyship->mx = skyship->mx - speed;
         break;

      case DIR_NORTHEAST:
         pair->mx = pair->mx + speed;
         pair->my = pair->my - speed;
         if( pair == ch )
         {
            skyship->mx = skyship->mx + speed;
            skyship->my = skyship->my - speed;
         }
         break;

      case DIR_NORTHWEST:
         pair->mx = pair->mx - speed;
         pair->my = pair->my - speed;
         if( pair == ch )
         {
            skyship->mx = skyship->mx - speed;
            skyship->my = skyship->my - speed;
         }
         break;

      case DIR_SOUTHEAST:
         pair->mx = pair->mx + speed;
         pair->my = pair->my + speed;
         if( pair == ch )
         {
            skyship->mx = skyship->mx + speed;
            skyship->my = skyship->my + speed;
         }
         break;

      case DIR_SOUTHWEST:
         pair->mx = pair->mx - speed;
         pair->my = pair->my + speed;
         if( pair == ch )
         {
            skyship->mx = skyship->mx - speed;
            skyship->my = skyship->my + speed;
         }
         break;

      default:
         break;
   }

   collect_followers( ch, ch->in_room, ch->in_room );

   /*
    * Reversed the order of these calls because of how the Overland clears the screen - Samson 
    */
   if( skyship->has_actflag( ACT_BOARDED ) )
      interpret( pair, "look" );

   /*
    * If skyship is looking for a better spot....
    */
   if( skyship->backtracking )
      land_skyship( pair, skyship, false );

   return;
}

/*
 * Create a skyship 
 */
void create_skyship( char_data * ch )
{
   mob_index *vskyship = NULL;
   char_data *skyship;

   if( !( vskyship = get_mob_index( MOB_VNUM_SKYSHIP ) ) )
   {
      bug( "%s: Vnum %d not found for skyship", __FUNCTION__, MOB_VNUM_SKYSHIP );
      return;
   }

   skyship = vskyship->create_mobile(  );

   /*
    * pick a random set of coordinates 
    */
   /*
    * and create skyship at the coords 
    */
   /*
    * skyship can be safely spawned using the same room as the PC calling it - Samson 
    */
   if( !skyship->to_room( ch->in_room ) )
      log_printf( "char_to_room: %s:%s, line %d.", __FILE__, __FUNCTION__, __LINE__ );
   skyship->set_actflag( ACT_ONMAP );
   skyship->inflight = true;
   skyship->heading = -1;
   skyship->map = ch->map;
   skyship->mx = ch->mx;
   skyship->my = ch->my;

   /*
    * Bond the player and skyship together 
    */
   ch->my_skyship = skyship;
   skyship->my_rider = ch;

   /*
    * Set the launch coords for backtracking, if needed later 
    */
   skyship->lcoordx = ch->mx;
   skyship->lcoordy = ch->my;

   /*
    * fly skyship to player location 
    */
   skyship->dcoordx = ch->mx;
   skyship->dcoordy = ch->my;

   add_event( 3, ev_skyship, skyship );
   fly_skyship( NULL, skyship );
   return;
}

/*
 * Call a skyship 
 */
CMDF( do_call )
{
   short terrain = get_terrain( ch->map, ch->mx, ch->my );

   /*
    * Sanity checks Reasons why a skyship wouldn't want to answer
    */
   /*
    * You a smelly mobbie?? 
    */
   if( ch->isnpc(  ) )
   {
      ch->print( "Sorry, mobs cannot use skyships to get around.\r\n" );
      return;
   }

   /*
    * Simplifies things to keep skyships on the Overland only - Samson 
    */
   if( !ch->has_pcflag( PCFLAG_ONMAP ) )
   {
      ch->print( "Skyships may only be called on the Overland.\r\n" );
      return;
   }

   /*
    * already has a skyship 
    */
   if( ch->my_skyship )
   {
      ch->print( "You have already sent for a skyship, be patient.\r\n" );
      return;
   }

   if( terrain != SECT_LANDING )
   {
      ch->print( "A skyship will not land except at a designated landing site.\r\n" );
      return;
   }
   ch->print( "You send for a skyship.\r\n" );
   act( AT_PLAIN, "$n sends for a skyship.", ch, NULL, NULL, TO_ROOM );
   create_skyship( ch );
   return;
}

landing_data *check_landing_site( short map, short x, short y )
{
   list<landing_data*>::iterator ilanding;

   for( ilanding = landinglist.begin(); ilanding != landinglist.end(); ++ilanding )
   {
      landing_data *landing = (*ilanding);

      if( landing->map == map )
      {
         if( landing->mx == x && landing->my == y )
            return landing;
      }
   }
   return NULL;
}

/*
 * Command to Fly a skyship 
 */
CMDF( do_fly )
{
   char_data *skyship = NULL;
   landing_data *lsite = NULL;
   int cost = 0;

   if( !ch->has_pcflag( PCFLAG_ONMAP ) )
   {
      ch->print( "This command can only be used from the Overland.\r\n" );
      return;
   }

   if( !ch->my_skyship )
   {
      ch->print( "You have not called for a skyship, how do you expect to do that?\r\n" );
      return;
   }

   if( !ch->has_pcflag( PCFLAG_BOARDED ) )
   {
      ch->print( "You aren't on a skyship.\r\n" );
      return;
   }

   skyship = ch->my_skyship;

   if( !is_same_char_map( skyship, ch ) )
   {
      ch->print( "The skyship you boarded is not here?!?!?\r\n" );
      return;
   }

   if( !argument || argument[0] == '\0' )
   {
      ch->print( "You need to specify a destination first.\r\n" );
      return;
   }

   lsite = check_landing_site( ch->map, ch->mx, ch->my );

   list<landing_data *>::iterator ilanding;
   for( ilanding = landinglist.begin(); ilanding != landinglist.end(); ++ilanding )
   {
      landing_data *landing = (*ilanding);

      if( landing->area && !str_prefix( argument, landing->area ) )
      {
         if( lsite && !str_cmp( landing->area, lsite->area ) )
         {
            ch->printf( "You are already at %s though!\r\n", argument );
            return;
         }

         if( landing->map != ch->map )
         {
            /*
             * Simplifies things. Especially since it would look funny to see alien terrain below you - Samson 
             */
            ch->print( "The skyship pilot refuses to fly you to another continent.\r\n" );
            return;
         }
         cost = landing->cost;

         if( ch->gold < cost )
         {
            ch->printf( "A flight to %s will cost you %d, which you cannot afford right now.\r\n",
                        landing->area, landing->cost );
            return;
         }
         ch->gold -= cost;

         ch->printf( "The skyship pilot takes your gold and charts a course to %s.\r\n", landing->area );
         skyship->dcoordx = landing->mx;
         skyship->dcoordy = landing->my;
         skyship->backtracking = false;
         ch->inflight = true;
         add_event( 3, ev_skyship, ch );
         ch->print_room( "The skyship ascends and takes to the wind.\r\n" );
         return;
      }
   }
   ch->printf( "There is no landing site in the vicinity of %s.\r\n", argument );
   skyship->dcoordx = ch->mx;
   skyship->dcoordy = ch->my;
   return;
}

CMDF( do_board )
{
   char_data *skyship = ch->my_skyship;

   if( !ch->has_pcflag( PCFLAG_ONMAP ) )
   {
      ch->print( "This command can only be used from the Overland.\r\n" );
      return;
   }

   if( !skyship )
   {
      ch->print( "You have not called for a skyship, how do you expect to do that?\r\n" );
      return;
   }

   if( !is_same_char_map( ch, skyship ) )
   {
      ch->print( "You cannot board yet, the skyship hasn't arrived.\r\n" );
      return;
   }

   if( ch->has_pcflag( PCFLAG_BOARDED ) )
   {
      ch->print( "You have already boarded the skyship.\r\n" );
      return;
   }
   skyship->set_actflag( ACT_BOARDED );
   ch->set_pcflag( PCFLAG_BOARDED );
   ch->print( "You climb abord the skyship and settle into your seat.\r\n" );
   return;
}

/*
 * Landing Site Stuff
 *   I take no credit for originality here on down.  This is 
 *   DIRECTLY "ADAPTED" from Samson's landmark code in overland.c
 */
landing_data::landing_data()
{
   init_memory( &area, &my, sizeof( my ) );
}

landing_data::~landing_data()
{
   STRFREE( area );
   landinglist.remove( this );
}

void fread_landing_sites( landing_data *landing, FILE * fp )
{
   for( ;; )
   {
      const char *word = ( feof( fp ) ? "End" : fread_word( fp ) );

      if( word[0] == '\0' )
      {
         bug( "%s: EOF encountered reading file!", __FUNCTION__ );
         word = "End";
      }

      switch ( UPPER( word[0] ) )
      {
         default:
            bug( "%s: no match: %s", __FUNCTION__, word );
            fread_to_eol( fp );
            break;

         case '*':
            fread_to_eol( fp );
            break;

         case 'A':
            KEY( "Area", landing->area, fread_string( fp ) );
            break;

         case 'C':
            if( !str_cmp( word, "Coordinates" ) )
            {
               landing->map = fread_short( fp );
               landing->mx = fread_short( fp );
               landing->my = fread_short( fp );
               break;
            }
            KEY( "Cost", landing->cost, fread_number( fp ) );
            break;

         case 'E':
            if( !str_cmp( word, "End" ) )
               return;
            break;
      }
   }
}

void load_landing_sites( void )
{
   char filename[256];
   landing_data *landing;
   FILE *fp;

   landinglist.clear();

   snprintf( filename, 256, "%s%s", MAP_DIR, LANDING_SITE_FILE );

   if( ( fp = fopen( filename, "r" ) ) != NULL )
   {
      for( ;; )
      {
         char letter;
         char *word;

         letter = fread_letter( fp );
         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }

         if( letter != '#' )
         {
            bug( "%s: # not found.", __FUNCTION__ );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "LANDING_SITE" ) )
         {
            landing = new landing_data;
            fread_landing_sites( landing, fp );
            landinglist.push_back( landing );
            continue;
         }
         else if( !str_cmp( word, "END" ) )
            break;
         else
         {
            bug( "%s: bad section: %s.", __FUNCTION__, word );
            continue;
         }
      }
      FCLOSE( fp );
   }
   return;
}

void save_landing_sites( void )
{
   FILE *fp;
   char filename[256];

   snprintf( filename, 256, "%s%s", MAP_DIR, LANDING_SITE_FILE );

   if( !( fp = fopen( filename, "w" ) ) )
   {
      bug( "%s: fopen", __FUNCTION__ );
      perror( filename );
   }
   else
   {
      list<landing_data*>::iterator ilanding;
      for( ilanding = landinglist.begin(); ilanding != landinglist.end(); ++ilanding )
      {
         landing_data *landing = (*ilanding);

         fprintf( fp, "%s", "#LANDING_SITE\n" );
         fprintf( fp, "Coordinates	%d %d %d\n", landing->map, landing->mx, landing->my );
         if( landing->area && landing->area[0] != '\0' )
            fprintf( fp, "Area	%s~\n", landing->area );
         fprintf( fp, "Cost	%d\n", landing->cost );
         fprintf( fp, "%s", "End\n\n" );
      }
      fprintf( fp, "%s", "#END\n" );
      FCLOSE( fp );
   }
   return;
}

void add_landing( short map, short x, short y )
{
   landing_data *landing;

   landing = new landing_data;
   landing->map = map;
   landing->mx = x;
   landing->my = y;
   landing->cost = 50000;
   landinglist.push_back( landing );
   save_landing_sites(  );
   return;
}

void delete_landing_site( landing_data *landing )
{
   if( !landing )
   {
      bug( "%s: Trying to delete NULL landing site.", __FUNCTION__ );
      return;
   }

   deleteptr( landing );

   if( !mud_down )
      save_landing_sites(  );
   return;
}

void free_landings( void )
{
   list<landing_data*>::iterator lands;

   for( lands = landinglist.begin(); lands != landinglist.end(); )
   {
      landing_data *landing = (*lands);
      ++lands;

      delete_landing_site( landing );
   }
   return;
}

/* Support command to list all landing sites currently loaded */
CMDF( do_landing_sites )
{
   list<landing_data*>::iterator ilanding;

   if( landinglist.empty() )
   {
      ch->print( "No landing sites defined.\r\n" );
      return;
   }

   ch->pager( "Continent | Coordinates | Area             | Cost     \r\n" );
   ch->pager( "------------------------------------------------------\r\n" );

   for( ilanding = landinglist.begin(); ilanding != landinglist.end(); ++ilanding )
   {
      landing_data *landing = (*ilanding);

      ch->pagerf( "%-10s  %-4dX %-4dY   %-15s   %d\r\n",
                  map_names[landing->map], landing->mx, landing->my, landing->area, landing->cost );
   }
   return;
}

/* OLC command to add/delete/edit landing site information */
CMDF( do_setlanding )
{
   landing_data *landing = NULL;
   char arg[MIL];

   if( ch->isnpc(  ) )
   {
      ch->print( "Sorry, NPCs have to walk.\r\n" );
      return;
   }

   if( !ch->has_pcflag( PCFLAG_ONMAP ) )
   {
      ch->print( "This command can only be used from the overland.\r\n" );
      return;
   }

   argument = one_argument( argument, arg );

   if( !arg || arg[0] == '\0' || !str_cmp( arg, "help" ) )
   {
      ch->print( "Usage: setlanding add\r\n" );
      ch->print( "Usage: setlanding delete\r\n" );
      ch->print( "Usage: setlanding area <area name>\r\n" );
      ch->print( "Usage: setlanding cost <cost>\r\n" );
      return;
   }

   landing = check_landing_site( ch->map, ch->mx, ch->my );

   if( !str_cmp( arg, "add" ) )
   {
      if( landing )
      {
         ch->print( "There's already a landing site at this location.\r\n" );
         return;
      }
      add_landing( ch->map, ch->mx, ch->my );
      putterr( ch->map, ch->mx, ch->my, SECT_LANDING );
      ch->print( "Landing site added.\r\n" );
      return;
   }

   if( !landing )
   {
      ch->print( "There is no landing site here.\r\n" );
      return;
   }

   if( !str_cmp( arg, "delete" ) )
   {
      delete_landing_site( landing );
      putterr( ch->map, ch->mx, ch->my, SECT_OCEAN );
      ch->print( "Landing site deleted.\r\n" );
      return;
   }

   if( !str_cmp( arg, "area" ) )
   {
      smash_tilde( argument );
      STRFREE( landing->area );
      landing->area = STRALLOC( argument );
      save_landing_sites(  );
      ch->printf( "Area set to %s.\r\n", argument );
      return;
   }

   if( !str_cmp( arg, "cost" ) )
   {
      landing->cost = atoi( argument );
      save_landing_sites(  );
      ch->printf( "Landing site cost set to %d\r\n", landing->cost );
      return;
   }
   do_setlanding( ch, "" );
   return;
}
