//
// Copyright (c) 2011-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/tetris
//

fn main()
{
   let word = "[|]";
   let esc = "\x1B";
   let reset = "\x1B[0m";
   let types = "1m   2m   3m   4m   5m   6m   7m";
   let background = "40m    41m    42m    43m    44m    45m    46m    47m";
   let background_bold = "1;40m  1;41m  1;42m  1;43m  1;44m  1;45m  1;46m  1;47m";
   let fg_color = ["30", "31", "32", "33", "34", "35", "36", "37", "1;30", "1;31", "1;32", "1;33", "1;34", "1;35", "1;36", "1;37"];
   let bg_color = ["40", "41", "42", "43", "44", "45", "46", "47", "1;40", "1;41", "1;42", "1;43", "1;44", "1;45", "1;46", "1;47", "0", "1", "2", "3", "4", "5", "6", "7", "8"];

   println!("            {}   {}   0m    {}", background, background_bold, types);

   for fg in fg_color.iter()
   {
        for _ in 0..5 - fg.len()
        {
            print!(" ");
        }
        print!("{}{}[{}m {} ", fg, esc, fg, word);
        let mut n = 0;
        for (_, bg) in bg_color.iter().enumerate()
        {
             if n > 16
             {
                print!("{}[{};{}m {} {}", esc, fg, bg, word, reset);
             }
             else
             {
                print!("{}[{};{}m  {}  {}", esc, fg, bg, word, reset);
             }
             n = n + 1;
        }
        println!();
   }

   println!();

   for m in 0..256
   {
       if (m+1)%32 == 1
       {
           print!("\t\t");
       }
       print!("{}[38;5;{};1m[]{}[0m", esc, m, esc);
       if (m+1)%32 == 0
       {
           print!("   |   ");
           for k in m-31..m+1
           {
               print!("{}[48;5;{};1m[]{}[0m", esc, k, esc);
           }
           println!();
       }
   }
}
