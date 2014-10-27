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
 *                        Finger and Wizinfo Module                         *
 ****************************************************************************/

#ifndef __FINGER_H__
#define __FINGER_H__

extern char *const realm_string[];  /* Wizinfo stuff */

class wizinfo_data
{
 private:
   wizinfo_data( const wizinfo_data& w );
   wizinfo_data& operator=( const wizinfo_data& );

 public:
   wizinfo_data();
   ~wizinfo_data();

   void set_name( string newname ) { name = newname; }
   string get_name() { return name; }

   void set_email( string newmail ) { email = newmail; }
   string get_email() { return email; }

   void set_icq( int num ) { icq = num; }
   int get_icq() { return icq; }

   void set_realm( short num ) { realm = num; }
   short get_realm() { return realm; }

   void set_level( short num ) { level = num; }
   short get_level() { return level; }

private:
   string name;
   string email;
   int icq;
   short realm;
   short level;
};
#endif
